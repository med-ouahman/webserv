config        ::= server_block+

server_block  ::= "server" "{" server_directive* "}"

server_directive ::= 
      listen_directive
    | server_name_directive
    | root_directive
    | index_directive
    | error_page_directive
    | client_max_body_size_directive
    | location_block

location_block ::= "location" path "{" location_directive* "}"

location_directive ::=
      root_directive
    | index_directive
    | allowed_methods_directive
    | autoindex_directive
    | return_directive
    | upload_store_directive
    | cgi_pass_directive
