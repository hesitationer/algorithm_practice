from wsgiref.simple_server import make_server
from cgi import parse_qs, escape
import json
import sae

# import ZtoK module
#import sys
#sys.path.insert(0,"../../../src")
import ZtoK

#for debug
import time

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

#Download_URL = "http://ztok.sinaapp.com/static/"
Download_URL = "http://localhost:8080/static/"
def get_input_post(environ, start_response):  
  
   # the environment variable CONTENT_LENGTH may be empty or missing  
   try:  
      request_body_size = int(environ.get('CONTENT_LENGTH', 0))  
   except (ValueError):  
      request_body_size = 0  
      print "Eric say:\n illegal request!\n"

      status = '200 OK'  
      response_headers = [('Content-type', 'text/plain')]
      start_response(status, response_headers)  
      return ["Eric say:\n illegal request!\n"]

   request_body = environ['wsgi.input'].read(request_body_size)  
   args_dict_t =  json.loads(request_body)
      
   # In this idiom you must issue a list containing a default value.  
   key1 = "url"
   key2 = "id"
   if key1 in args_dict_t:
       url = args_dict_t['url'] # Returns the first url value.  

       # Always escape user input to avoid script injection  
       url = escape(url)
       # process by ZtoK.py
       result =  ZtoK.get_authors(url)

   elif key2 in args_dict_t:
       choosed_id = args_dict_t['id']

       # Always escape user input to avoid script injection  
       #choosed_id = escape(choosed_id)
       # process by ZtoK.py
       print "get_answers  ", choosed_id
       mobi = ZtoK.get_answers(choosed_id)
       print type(Download_URL)
       print type(mobi)
       print type("path")
       result = {"downloadpath":Download_URL + str(mobi)} 
   else:
       print "unknow args!"
       return ['Eric Said:\n\tNoNo,illegal request!\n']
  
   response_body = json.dumps(result)
   
   status = '200 OK'  
   # Now content type is text/html  
   response_headers = [('Content-Type', 'text/html'),  
                  ('Content-Length', str(len(response_body)))]  
   start_response(status, response_headers)  
  
   #print type([response_body])
   print time.asctime( time.localtime(time.time()))
   return [response_body]  

def app(environ, start_response):
    status = '200 OK'
    response_headers = [('Content-type', 'text/plain')]
    start_response(status, response_headers)
    return ['Hello, Yangyaqi!']

application = sae.create_wsgi_app(get_input_post)
