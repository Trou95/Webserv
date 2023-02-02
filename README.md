**A simple webserver written in c++**

- Can serve static html pages
- Can run multiple CGI(Php,Python etc)
- Able File Uploads
- And Also Handle Cookies and Sessions

## Configuration File

**listen**

    listen port_number

**root**

    root root_path
    
**server_name**

    server_name name

**error_page**

    error_page path_to_error_page

client_limit

    client_limit limit_to_body_size
  

**Locations**

    location /path {
    
    index index.html
    allowed_methods GET
    
    }

