# Resource Path Resolution — URI to Filesystem Mapping

## Core Problem

Given:
- HTTP request URI: `/static/images/logo.png`
- Location configuration

Determine:
- Actual filesystem path: `/var/www/assets/images/logo.png`

This mapping is **not trivial** and depends on configuration directives.

---

## When Does Path Resolution Happen?

**TIMING:** After request parsing, before response type determination.

```
Request Parsed
    ↓
Match Location Block
    ↓
Resolve Resource Path ← YOU ARE HERE
    ↓
Determine Response Type
    ↓
Generate Response
```

**Why this order?**

Response type determination needs the actual filesystem path to check:
- Does the path exist?
- Is it a file or directory?
- Is it readable?
- Does it match CGI patterns?

---

## The Two Path Directives: `root` vs `alias`

### `root` Directive (Append Mode)

**Behavior:** Appends the **full request URI** to the root path.

**Syntax:**
```nginx
location /prefix {
    root /filesystem/path;
}
```

**Resolution:**
```
filesystem_path = root + request_uri
```

**Example 1:**
```nginx
location / {
    root /var/www;
}

Request: GET /images/logo.png
Result:  /var/www/images/logo.png
         └─root─┘└──request_uri──┘
```

**Example 2:**
```nginx
location /static {
    root /var/www;
}

Request: GET /static/css/style.css
Result:  /var/www/static/css/style.css
         └─root─┘└──request_uri─────┘
```

**Key Point:** The location prefix (`/static`) is **included** in the final path.

---

### `alias` Directive (Replace Mode)

**Behavior:** **Replaces** the location prefix with the alias path.

**Syntax:**
```nginx
location /prefix {
    alias /filesystem/path;
}
```

**Resolution:**
```
remaining_uri = request_uri - location_prefix
filesystem_path = alias + remaining_uri
```

**Example 1:**
```nginx
location /static {
    alias /var/www/assets;
}

Request: GET /static/css/style.css
         └─prefix─┘└─remaining─┘

Result:  /var/www/assets/css/style.css
         └───alias────┘└─remaining─┘
```

**Example 2:**
```nginx
location /downloads {
    alias /mnt/storage/files;
}

Request: GET /downloads/report.pdf
Result:  /mnt/storage/files/report.pdf
         (NOT /mnt/storage/files/downloads/report.pdf)
```

**Key Point:** The location prefix is **removed** before appending to the alias path.

---

## Root vs Alias: When to Use Which?

### Use `root` When:

1. **Filesystem mirrors URI structure**

```
/var/www/
    ├── images/
    ├── css/
    └── js/

location / {
    root /var/www;
}

GET /images/logo.png → /var/www/images/logo.png ✓
```

2. **Multiple locations share the same root**

```
location / {
    root /var/www;
}

location /admin {
    root /var/www;
}

GET /index.html       → /var/www/index.html
GET /admin/panel.html → /var/www/admin/panel.html
```

---

### Use `alias` When:

1. **Filesystem structure differs from URI**

```
/opt/assets/
    ├── stylesheets/
    ├── images/
    └── scripts/

location /static {
    alias /opt/assets;
}

GET /static/images/logo.png → /opt/assets/images/logo.png
(NOT /opt/assets/static/images/logo.png)
```

2. **Mapping a single URI to a different location**

```
location /downloads {
    alias /mnt/external-drive/shared;
}

GET /downloads/file.zip → /mnt/external-drive/shared/file.zip
```

3. **Hiding internal structure**

```
location /docs {
    alias /usr/share/company-docs/public;
}

GET /docs/manual.pdf → /usr/share/company-docs/public/manual.pdf
```

---

## Path Resolution Algorithm

### Step 1: Match Location Block

```cpp
Location* match_location(string uri, vector<Location> locations) {
    Location* best_match = nullptr;
    size_t best_length = 0;
    
    for (Location& loc : locations) {
        if (uri.starts_with(loc.prefix)) {
            if (loc.prefix.length() > best_length) {
                best_match = &loc;
                best_length = loc.prefix.length();
            }
        }
    }
    
    return best_match ? best_match : &default_location;
}
```

**Matching Rules:**
1. Exact match (`=` modifier) wins immediately
2. Longest prefix match wins
3. First regex match wins (if no prefix match)
4. Default location (`/`) as fallback

---

### Step 2: Construct Filesystem Path

```cpp
string resolve_path(string uri, Location loc) {
    
    if (loc.has_alias()) {
        // ALIAS mode: replace prefix
        
        string remaining = uri.substr(loc.prefix.length());
        return loc.alias + remaining;
    }
    else {
        // ROOT mode: append full URI
        
        return loc.root + uri;
    }
}
```

---

### Step 3: Sanitize Path

**Critical security step:** Prevent directory traversal attacks.

```cpp
string sanitize_path(string path) {
    
    // 1. Normalize path (resolve . and ..)
    path = normalize_path(path);
    
    // 2. Verify path is within allowed root
    if (!is_within_root(path, allowed_root)) {
        throw SecurityException("Path traversal attempt");
    }
    
    // 3. Check for null bytes
    if (path.contains('\0')) {
        throw SecurityException("Null byte in path");
    }
    
    return path;
}
```

**Example Attack:**
```
Request: GET /../../../etc/passwd
Naive path: /var/www/../../../etc/passwd
Normalized: /etc/passwd ← OUTSIDE web root!

Defense: Reject if normalized path doesn't start with /var/www
```

---

## Complete Resolution Function

```cpp
struct ResolvedPath {
    string filesystem_path;
    Location* location;
    bool valid;
    int error_code;  // 0 if valid, HTTP error code otherwise
};

ResolvedPath resolve_resource_path(RequestData& request, Config& config) {
    
    ResolvedPath result;
    
    // Step 1: Match location block
    result.location = match_location(request.uri, config.locations);
    if (!result.location) {
        result.valid = false;
        result.error_code = 500;  // Configuration error
        return result;
    }
    
    // Step 2: Construct path
    string raw_path;
    
    if (result.location->has_alias()) {
        // Alias: replace prefix
        string remaining = request.uri.substr(result.location->prefix.length());
        raw_path = result.location->alias + remaining;
    }
    else {
        // Root: append URI
        raw_path = result.location->root + request.uri;
    }
    
    // Step 3: Sanitize
    try {
        result.filesystem_path = sanitize_path(raw_path);
    }
    catch (SecurityException& e) {
        result.valid = false;
        result.error_code = 403;  // Forbidden
        return result;
    }
    
    // Step 4: Validate within web root
    if (!is_within_allowed_root(result.filesystem_path, result.location->root)) {
        result.valid = false;
        result.error_code = 403;
        return result;
    }
    
    result.valid = true;
    result.error_code = 0;
    return result;
}
```

---

## Edge Cases and Special Handling

### 1. Trailing Slash in Location Prefix

**Problem:** Does `/static` match `/static/file` or only `/static/`?

**Solution:** Support both, but be consistent.

**Recommended Behavior:**
```nginx
location /static {
    alias /var/www/assets;
}

GET /static      → should redirect to /static/
GET /static/     → /var/www/assets/
GET /staticfile  → does NOT match (prefix is /static, not /static*)
```

**Implementation:**
```cpp
bool matches_location(string uri, string prefix) {
    if (uri == prefix) {
        return true;  // Exact match
    }
    
    if (uri.starts_with(prefix + "/")) {
        return true;  // Prefix with separator
    }
    
    return false;
}
```

---

### 2. Query Strings

**Rule:** Query strings are **not** part of the path.

**Example:**
```
Request: GET /search?q=test
URI for path resolution: /search
Query string: q=test (handled separately)
```

**Implementation:**
```cpp
string extract_path(string full_uri) {
    size_t query_pos = full_uri.find('?');
    if (query_pos != string::npos) {
        return full_uri.substr(0, query_pos);
    }
    return full_uri;
}
```

---

### 3. URL Encoding

**Rule:** Decode URI before path resolution.

**Example:**
```
Raw request: GET /files/my%20document.pdf
Decoded URI: /files/my document.pdf
Filesystem:  /var/www/files/my document.pdf
```

**Implementation:**
```cpp
string url_decode(string uri) {
    string result;
    
    for (size_t i = 0; i < uri.length(); i++) {
        if (uri[i] == '%' && i + 2 < uri.length()) {
            string hex = uri.substr(i + 1, 2);
            result += (char)stoi(hex, nullptr, 16);
            i += 2;
        }
        else if (uri[i] == '+') {
            result += ' ';
        }
        else {
            result += uri[i];
        }
    }
    
    return result;
}
```

**Security Note:** Decode **before** sanitization to catch encoded traversal attempts:
```
Bad: /%2e%2e%2f → decoded → /../
```

---

### 4. Index File Resolution

**When:** Path is a directory.

**Example:**
```
Request: GET /docs/
Filesystem: /var/www/docs/ (directory)

Check for:
    /var/www/docs/index.html
    /var/www/docs/index.htm
    
If found: Use that file
If not found: Directory listing or 403
```

**Implementation:**
```cpp
string resolve_index_file(string dir_path, vector<string> index_files) {
    for (const string& index : index_files) {
        string full_path = dir_path + "/" + index;
        if (file_exists(full_path)) {
            return full_path;
        }
    }
    return "";  // No index found
}
```

---

### 5. CGI Script Path Resolution

**Special case:** CGI scripts may need both:
- **Filesystem path** (to execute the script)
- **Path info** (additional path after script name)

**Example:**
```
Request: GET /cgi-bin/script.py/extra/path

Script path: /var/www/cgi-bin/script.py
Path info:   /extra/path
```

**Implementation:**
```cpp
struct CGIPath {
    string script_path;
    string path_info;
};

CGIPath resolve_cgi_path(string uri, Location loc) {
    CGIPath result;
    
    // Find where the script ends
    string base_path = loc.root + uri;
    
    // Walk backwards to find executable file
    string current = base_path;
    while (!current.empty()) {
        if (file_exists(current) && is_executable(current)) {
            result.script_path = current;
            result.path_info = base_path.substr(current.length());
            return result;
        }
        
        // Remove last component
        size_t pos = current.find_last_of('/');
        if (pos == string::npos) break;
        current = current.substr(0, pos);
    }
    
    return result;  // Not found
}
```

---

## Path Normalization

**Purpose:** Resolve `.` and `..` components safely.

**Algorithm:**

```cpp
string normalize_path(string path) {
    vector<string> components;
    istringstream stream(path);
    string component;
    
    while (getline(stream, component, '/')) {
        if (component.empty() || component == ".") {
            continue;  // Skip
        }
        else if (component == "..") {
            if (!components.empty()) {
                components.pop_back();  // Go up one level
            }
            // If components is empty, we're trying to go above root
            // This should be caught by security checks
        }
        else {
            components.push_back(component);
        }
    }
    
    // Reconstruct path
    string result = "/";
    for (size_t i = 0; i < components.size(); i++) {
        result += components[i];
        if (i < components.size() - 1) {
            result += "/";
        }
    }
    
    return result;
}
```

**Examples:**
```
/a/b/../c       → /a/c
/a/./b          → /a/b
/a//b           → /a/b
/a/b/../../c    → /c
/../etc/passwd  → /etc/passwd (caught by security check)
```

---

## Security Validation

```cpp
bool is_within_allowed_root(string path, string allowed_root) {
    
    // Normalize both paths
    string norm_path = normalize_path(path);
    string norm_root = normalize_path(allowed_root);
    
    // Ensure root ends with /
    if (!norm_root.ends_with('/')) {
        norm_root += '/';
    }
    
    // Check if path starts with root
    return norm_path.starts_with(norm_root);
}
```

**Test Cases:**
```
Path: /var/www/files/document.pdf
Root: /var/www
Result: ALLOWED

Path: /var/www/../etc/passwd
Normalized: /var/etc/passwd
Root: /var/www
Result: DENIED

Path: /var/www/files/../../etc/passwd
Normalized: /var/etc/passwd
Root: /var/www
Result: DENIED
```

---

## Integration with Response Type Resolution

```cpp
ResponseContext process_request(RequestData& req, Config& config) {
    
    ResponseContext ctx;
    
    // 1. Resolve resource path
    ResolvedPath resolved = resolve_resource_path(req, config);
    
    if (!resolved.valid) {
        ctx.type = ERROR_RESPONSE;
        ctx.status_code = resolved.error_code;
        return ctx;
    }
    
    // 2. Store resolved path
    ctx.filesystem_path = resolved.filesystem_path;
    ctx.location = resolved.location;
    
    // 3. Determine response type (using filesystem path)
    ctx.type = determine_response_type(req, resolved);
    
    return ctx;
}
```

---

## Configuration Examples

### Example 1: Simple Static Site

```nginx
server {
    listen 8080;
    server_name example.com;
    
    location / {
        root /var/www/html;
        index index.html index.htm;
    }
}
```

**Requests:**
```
GET /                 → /var/www/html/index.html
GET /about.html       → /var/www/html/about.html
GET /images/logo.png  → /var/www/html/images/logo.png
```

---

### Example 2: Separate Static Assets

```nginx
server {
    listen 8080;
    
    location / {
        root /var/www/html;
    }
    
    location /static {
        alias /opt/cdn/assets;
    }
}
```

**Requests:**
```
GET /index.html        → /var/www/html/index.html
GET /static/style.css  → /opt/cdn/assets/style.css
```

---

### Example 3: Multiple Document Roots

```nginx
server {
    listen 8080;
    
    location /public {
        alias /var/www/public;
    }
    
    location /private {
        alias /var/www/private;
        # Add authentication here
    }
    
    location /downloads {
        alias /mnt/storage/files;
    }
}
```

**Requests:**
```
GET /public/info.txt    → /var/www/public/info.txt
GET /private/secret.txt → /var/www/private/secret.txt
GET /downloads/file.zip → /mnt/storage/files/file.zip
```

---

### Example 4: CGI with Separate Location

```nginx
server {
    listen 8080;
    
    location / {
        root /var/www/html;
    }
    
    location /cgi-bin {
        root /var/www;
        cgi_enabled on;
        cgi_extension .py;
    }
}
```

**Requests:**
```
GET /index.html           → /var/www/html/index.html
GET /cgi-bin/script.py    → /var/www/cgi-bin/script.py (execute)
```

---

## Common Mistakes

### Mistake 1: Confusing root and alias

**Wrong:**
```nginx
location /static {
    root /opt/assets;  # Wrong!
}

GET /static/style.css → /opt/assets/static/style.css
                                     ^^^^^^^^^ duplicate!
```

**Right:**
```nginx
location /static {
    alias /opt/assets;
}

GET /static/style.css → /opt/assets/style.css ✓
```

---

### Mistake 2: Forgetting to sanitize

**Vulnerable:**
```cpp
string path = config.root + request.uri;
// No normalization or validation!
```

**Attack:**
```
GET /../../../etc/passwd
Path: /var/www/../../../etc/passwd
Serves: /etc/passwd ← SECURITY BREACH!
```

**Fixed:**
```cpp
string path = resolve_and_sanitize(request.uri, config);
// Includes normalization and boundary checks
```

---

### Mistake 3: Not handling URL encoding

**Problem:**
```
Request: GET /files/my%20file.txt
Naive:   /var/www/files/my%20file.txt
Actual:  /var/www/files/my file.txt
Result:  404 (file not found)
```

**Fix:** Decode before path resolution.

---

### Mistake 4: Including query string in path

**Problem:**
```
Request: GET /search?q=test
Naive:   /var/www/search?q=test
Result:  File not found (? is not a valid filename)
```

**Fix:** Strip query string before path resolution.

---

## Implementation Checklist

- [ ] Implement location matching (exact, prefix, regex)
- [ ] Implement root directive path construction
- [ ] Implement alias directive path construction
- [ ] Implement URL decoding
- [ ] Implement path normalization (. and ..)
- [ ] Implement directory traversal prevention
- [ ] Implement query string extraction
- [ ] Implement index file resolution
- [ ] Implement CGI path info extraction
- [ ] Write security tests for directory traversal
- [ ] Write tests for all edge cases

---

## Summary

**Path Resolution happens in 4 steps:**

1. **Match location** (longest prefix wins)
2. **Construct path** (root = append, alias = replace)
3. **Sanitize** (decode, normalize, validate)
4. **Use for response type** (check existence, type, permissions)

**Key Rules:**

- `root` appends the full URI
- `alias` replaces the location prefix
- Always decode before sanitizing
- Always normalize before checking boundaries
- Query strings are separate from paths
- Index files are resolved for directories

This architecture ensures secure, predictable path resolution while supporting flexible configuration.
