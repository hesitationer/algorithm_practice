from wsgiref.simple_server import make_server
from cgi import parse_qs, escape
import json
import sae

html = """ 
<html> 
<body> 
   <form method="get" action=""> 
      <p> 
         Age: <input type="text" name="age"> 
         </p> 
      <p> 
         Hobbies: 
         <input name="hobbies" type="checkbox" value="software"> Software 
         <input name="hobbies" type="checkbox" value="tunning"> Auto Tunning 
         </p> 
      <p> 
         <input type="submit" value="Submit"> 
         </p> 
      </form> 
   <p> 
      Age: %s<br> 
      Hobbies: %s 
      </p> 
   </body> 
</html>"""  

def get_request_args(environ, start_response):
    d = parse_qs(environ['QUERY_STRING'])
    url = d.get('url',[])

    status = '200 OK'
    response_headers = [('Content-type', 'text/plain')]
    start_response(status, response_headers)

    #return ['from server']
    return url


def get_input_post(environ, start_response):  
  
   # the environment variable CONTENT_LENGTH may be empty or missing  
   try:  
      request_body_size = int(environ.get('CONTENT_LENGTH', 0))  
   except (ValueError):  
      request_body_size = 0  
  
   request_body = environ['wsgi.input'].read(request_body_size)  
   d = parse_qs(request_body)  
      
   # In this idiom you must issue a list containing a default value.  
   url = d.get('url', [''])[0] # Returns the first url value.  
  
   # Always escape user input to avoid script injection  
   url = escape(url)
  
   #response_body = html % (url or 'Empty',  
   #            ', '.join(hobbies or ['No Hobbies']))  
   response_body = url
   response_body = json.dumps({ 'firstAccess': "2014-06-06" })
   

   status = '200 OK'  
  
   # Now content type is text/html  
   response_headers = [('Content-Type', 'text/html'),  
                  ('Content-Length', str(len(response_body)))]  
   start_response(status, response_headers)  
  
   return [response_body]  

def app(environ, start_response):
    status = '200 OK'
    response_headers = [('Content-type', 'text/plain')]
    start_response(status, response_headers)
    return ['Hello, Yangyaqi!']

application = sae.create_wsgi_app(get_input_post)
