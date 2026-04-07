#!/usr/bin/env bash
# run_all_tests.sh — Build and run all HTTP parser test clients
#
# Usage:
#   ./run_all_tests.sh                   # defaults: host=127.0.0.1  port=8080
#   ./run_all_tests.sh 127.0.0.1 8080
#   ./run_all_tests.sh --skip rust       # skip one language
#
# Output:
#   Coloured per-language results + final summary table.
#   Exit 0 when all selected clients pass; non-zero otherwise.

set -euo pipefail

# ── defaults ──────────────────────────────────────────────────────────────────
HOST="127.0.0.1"
PORT="8080"
SKIP=()

# ── argument parsing ──────────────────────────────────────────────────────────
while [[ $# -gt 0 ]]; do
  case "$1" in
    --skip) SKIP+=("$2"); shift 2 ;;
    --host) HOST="$2";    shift 2 ;;
    --port) PORT="$2";    shift 2 ;;
    -h|--help)
      echo "Usage: $0 [--host H] [--port P] [--skip LANG]..."
      echo "  LANG ∈ python c cpp php rust js"
      exit 0 ;;
    *) shift ;;
  esac
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
mkdir -p "$BUILD_DIR"

# ── colours ───────────────────────────────────────────────────────────────────
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; RESET='\033[0m'

# ── result tracking ───────────────────────────────────────────────────────────
declare -A LANG_STATUS   # pass | fail | skip | error
declare -A LANG_TIME_MS

LANGS=(python c cpp php rust js)

# ── helpers ───────────────────────────────────────────────────────────────────

should_skip() {
  local lang="$1"
  for s in "${SKIP[@]+"${SKIP[@]}"}"; do
    [[ "$s" == "$lang" ]] && return 0
  done
  return 1
}

cmd_exists() { command -v "$1" &>/dev/null; }

elapsed_ms() {
  # $1 = start seconds (from date +%s%N or SECONDS)
  echo $(( ($(date +%s%N) - $1) / 1000000 ))
}

run_client() {
  local lang="$1"
  local cmd=("${@:2}")
  local start
  start=$(date +%s%N)

  printf "${CYAN}┌─ %s ─────────────────────────────────${RESET}\n" "$lang"
  if "${cmd[@]}" "$HOST" "$PORT"; then
    LANG_STATUS[$lang]="pass"
  else
    LANG_STATUS[$lang]="fail"
  fi
  LANG_TIME_MS[$lang]=$(elapsed_ms "$start")
  printf "${CYAN}└──────────────────────────────────────${RESET}\n"
}

# ── connectivity check ────────────────────────────────────────────────────────

printf "\n${BOLD}HTTP Parser Multi-Language Test Suite${RESET}\n"
printf "Target: %s:%s\n\n" "$HOST" "$PORT"

if ! nc -z -w2 "$HOST" "$PORT" 2>/dev/null; then
  printf "${RED}ERROR: Cannot reach %s:%s — start your server first.${RESET}\n" "$HOST" "$PORT"
  exit 1
fi
printf "${GREEN}✓ Server reachable at %s:%s${RESET}\n\n" "$HOST" "$PORT"

# ── Python ────────────────────────────────────────────────────────────────────

if should_skip python; then
  LANG_STATUS[python]="skip"
elif ! cmd_exists python3; then
  printf "${YELLOW}⚠  python3 not found — skipping${RESET}\n"
  LANG_STATUS[python]="skip"
else
  run_client "Python" python3 "${SCRIPT_DIR}/test_parser.py"
fi

# ── C ─────────────────────────────────────────────────────────────────────────

if should_skip c; then
  LANG_STATUS[c]="skip"
elif ! cmd_exists gcc; then
  printf "${YELLOW}⚠  gcc not found — skipping C${RESET}\n"
  LANG_STATUS[c]="skip"
else
  printf "${CYAN}Building C client...${RESET}\n"
  gcc -std=c11 -Wall -Wextra -O2 \
      -o "${BUILD_DIR}/test_parser_c" \
      "${SCRIPT_DIR}/test_parser.c" 2>&1 | sed 's/^/  /'
  run_client "C" "${BUILD_DIR}/test_parser_c"
fi

# ── C++ ───────────────────────────────────────────────────────────────────────

if should_skip cpp; then
  LANG_STATUS[cpp]="skip"
elif ! cmd_exists g++; then
  printf "${YELLOW}⚠  g++ not found — skipping C++${RESET}\n"
  LANG_STATUS[cpp]="skip"
else
  printf "${CYAN}Building C++ client...${RESET}\n"
  g++ -std=c++17 -Wall -Wextra -O2 \
      -o "${BUILD_DIR}/test_parser_cpp" \
      "${SCRIPT_DIR}/test_parser.cpp" 2>&1 | sed 's/^/  /'
  run_client "C++" "${BUILD_DIR}/test_parser_cpp"
fi

# ── PHP ───────────────────────────────────────────────────────────────────────

if should_skip php; then
  LANG_STATUS[php]="skip"
elif ! cmd_exists php; then
  printf "${YELLOW}⚠  php not found — skipping${RESET}\n"
  LANG_STATUS[php]="skip"
else
  run_client "PHP" php "${SCRIPT_DIR}/test_parser.php"
fi

# ── Rust ──────────────────────────────────────────────────────────────────────

if should_skip rust; then
  LANG_STATUS[rust]="skip"
elif ! cmd_exists rustc; then
  printf "${YELLOW}⚠  rustc not found — skipping Rust${RESET}\n"
  LANG_STATUS[rust]="skip"
else
  printf "${CYAN}Building Rust client...${RESET}\n"
  rustc -O -o "${BUILD_DIR}/test_parser_rs" \
        "${SCRIPT_DIR}/test_parser.rs" 2>&1 | sed 's/^/  /'
  run_client "Rust" "${BUILD_DIR}/test_parser_rs"
fi

# ── JavaScript (Node.js) ──────────────────────────────────────────────────────

if should_skip js; then
  LANG_STATUS[js]="skip"
elif ! cmd_exists node; then
  printf "${YELLOW}⚠  node not found — skipping JS${RESET}\n"
  LANG_STATUS[js]="skip"
else
  run_client "JavaScript" node "${SCRIPT_DIR}/test_parser.js"
fi

# ── summary table ─────────────────────────────────────────────────────────────

printf "\n${BOLD}══════════════════════════════════════${RESET}\n"
printf "${BOLD}  Summary${RESET}\n"
printf "${BOLD}══════════════════════════════════════${RESET}\n"
printf "  %-12s  %-6s  %s\n" "Language" "Status" "Time"
printf "  %-12s  %-6s  %s\n" "────────" "──────" "────"

overall_fail=0
for lang in "${LANGS[@]}"; do
  status="${LANG_STATUS[$lang]:-skip}"
  ms="${LANG_TIME_MS[$lang]:-—}"
  [[ "$ms" != "—" ]] && ms="${ms}ms"

  case "$status" in
    pass)  colour="${GREEN}"; sym="✓ pass" ;;
    fail)  colour="${RED}";   sym="✗ fail"; overall_fail=1 ;;
    skip)  colour="${YELLOW}"; sym="- skip" ;;
    error) colour="${RED}";   sym="! error"; overall_fail=1 ;;
    *)     colour="";         sym="?" ;;
  esac

  printf "  %-12s  ${colour}%-6s${RESET}  %s\n" "$lang" "$sym" "$ms"
done

printf "${BOLD}══════════════════════════════════════${RESET}\n"

if [[ $overall_fail -eq 0 ]]; then
  printf "${GREEN}  All selected clients passed.${RESET}\n"
else
  printf "${RED}  One or more clients failed.${RESET}\n"
fi
printf "\n"

exit $overall_fail
