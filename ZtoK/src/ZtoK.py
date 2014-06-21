# -*- coding=utf-8 -*-
import urllib2
import codecs
from bs4 import BeautifulSoup
from bs4 import Tag 
from bs4 import NavigableString

import sys 
reload(sys) 
sys.setdefaultencoding('utf8')

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
	f = open(jpg_name,'wb')
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
This function merge the 
	1. default_kindle.html
	2. the answer extracted from the page

and generate a new doc for  kindle.
"""
def generate_new_doc(question,answer):
	new_doc = open(question + ".html",'w+')

	head_tag = BeautifulSoup(open('default_kindle.html','r')).head
	answer.body.insert_before(head_tag)

	# set the header and css
	new_doc.write(answer.prettify())
	new_doc.close()



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

	# find answers
	answers = soup.find_all("div","fixed-summary zm-editable-content clearfix")
	i = 0
	for a in answers:
		i = i + 1
		if i == answer_choosed:
			print type(a)
			a = process_img_path(a) # NOTE: a changes from `tag` to `soup`
			print type(a)
			#print a.prettify()
			generate_new_doc('new_doc',a)

if __name__=="__main__":
	get_authors("http://www.zhihu.com/question/20459385");
	get_answers(3);
