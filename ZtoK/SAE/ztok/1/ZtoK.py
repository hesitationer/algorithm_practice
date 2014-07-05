# -*- coding=utf-8 -*-
import urllib2
import codecs
from bs4 import BeautifulSoup
from bs4 import Tag 
from bs4 import NavigableString

import shutil
import os
import sys 
reload(sys) 
sys.setdefaultencoding('utf8')

# call kindlegen
from subprocess import call


# Only test on form like this:
#
#    http://p4.zhimg.com/7f/15/7f1552a997bd5137b07446b6b24cde36_m.jpg
#
def get_jpg_name_from_url(url):
	return url.split('/')[-1]

def download_image(url):
	resp = urllib2.urlopen(url)
	data = resp.read()

	jpg_name = get_jpg_name_from_url(url)
	f = open('output/' + jpg_name,'wb')
	f.write(data)
	f.close

	return jpg_name

"""
img tag in ZhiHu answer looks likes this:

	<img
		src = "//s1.zhimg.com/misc/whitedot.jpg"
		data-rawwidth = "754"
		data-rawheight = "1250"
		class = "origin_image zh-lightbox-thumb lazy"
		width = "754"
		data-original = "http://p4.zhimg.com/7f/15/7f1552a997bd5137b07446b6b24cde36_r.jpg"
		data-actualsrc = "http://p4.zhimg.com/7f/15/7f1552a997bd5137b07446b6b24cde36_m.jpg">

This function set the `src` to the content of `data-actualsrc`
"""
def process_img_path(answer):
	soup = BeautifulSoup(unicode(answer))

	img_tags = soup.find_all('img')
	for i in img_tags:
		#print i
		#print i['src'],'\n\n'
		#if i['src'].startswith('//'):
		#	print 'True'
		#	print i['data-actualsrc']
		#	i['src'] = i['data-actualsrc']
		#else:
		#	print 'False'
		#print '--------------------'

		if i['src'].startswith('//'):
			local_file = download_image(i['data-actualsrc'])
			i['src'] = local_file

	return soup


"""
This function remove the tags like this:
	<noscript>
		<img class="origin_image zh-lightbox-thumb"
		data-original="http://pic2.zhimg.com/9ba57ec1d11cd202c78f1105d6ef99fe_r.jpg"
		data-rawheight="557" data-rawwidth="876"
		src="http://pic2.zhimg.com/9ba57ec1d11cd202c78f1105d6ef99fe_b.jpg"
		width="876"/>
	</noscript>
"""
def remove_noscript_tag(soup):
	
	no_script_tag = soup.find_all('noscript')
	for i in no_script_tag:
		i.extract()

	return soup

"""
This function merge the 
	1. default_kindle.html
	2. the answer extracted from the page

and generate a new doc for  kindle.
"""
def generate_new_doc(question,answer):

	print os.getcwd()

	# open file
	html = "output/" + question + ".html"
	new_doc = open(html,'w+')

	soup = BeautifulSoup(open('source/default_kindle.html','r'))


	# head
	head_tag = soup.head  
	answer.body.insert_before(head_tag)

	# title. used by kindlegen as the meta-data
	title_string = get_title()
	answer.title.string = title_string

	# Add <h>title</h>
	h1 = soup.new_tag("h2")
	h1.string = title_string
	answer.body.contents[0].insert_before(h1)

	# output
	new_doc.write(answer.prettify())
	new_doc.close()

	# convert to mobi
	call_kindlegen(html)
	print "generate_new_doc done"

	# move to static dir for downloading
	mobi = "./output/"+question+".mobi"
	if os.path.isfile(mobi):
		dst = "./static/"+question+".mobi"
		shutil.copy(mobi,dst)
		print dst




def ZtoK(z_url):
	print "not implement yet"

"""
This two belowing url used for test in development
Zurl = "http://www.zhihu.com/question/22001726"
Zurl = "http://www.zhihu.com/question/20459385"
"""
global soup  #I don't want this global variants too, trust me.
def get_authors(z_url):
	global soup
	Zurl = z_url
	Zpage = urllib2.urlopen(Zurl)

	soup = BeautifulSoup(Zpage)

	author_list = list()
	# find answer-owners
	head = soup.find_all("div","zm-item-answer-author-info")
	Num = 0
	for i in head: #all the divs of 'zm-item-answer-author-info'
		Num = Num + 1
		for j in i.children: # children in divs
			if isinstance(j,Tag):
				if j.name == 'h3': # h3-tag
					for k in j.children:
						if isinstance(k,Tag):# 
							if k.name == 'a' and not k.has_attr('class'):# user-link
								print Num, ":",
								print k.string;author_list.append(k.string)
								break
						if isinstance(k,NavigableString):
							if unicode(k).find(u'知乎用户') > 0: # k contains '知乎用户'
								print Num, ":",
								print "未知用户";author_list.append("未知用户")

	return author_list

def get_answers(answer_choosed):
	global soup

	print "in get_answers:", answer_choosed 

	# find answers
	answers = soup.find_all("div","fixed-summary zm-editable-content clearfix")
	i = 0
	for a in answers:
		i = i + 1
		if i == answer_choosed:
			print type(a)
			a = process_img_path(a) # NOTE: a changes from `tag` to `soup`
			a = remove_noscript_tag(a)
			print type(a)
			#print a.prettify()
			title = get_title()
			generate_new_doc(title,a)

			return title + ".mobi"

"""
This function get the string between
	<title>
			这是问题的标题？ - 知乎
	</title>

And split by '？'，return the first part.
"""

def get_title():
	global soup

	title = soup.find("title")
	t_list = title.string.strip('\n').split(u'？')

	print t_list 
	print len(t_list)
	for i in t_list:
		print i

	return t_list[0]
	

def call_kindlegen(input_html):
	print os.getcwd()
	kindlegen = "./kindlegen/kindlegen"
	rc = call([kindlegen, "", input_html])

if __name__=="__main__":
	get_authors("http://www.zhihu.com/question/20459385");
	get_answers(3);
