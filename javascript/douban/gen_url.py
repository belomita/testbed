#!/usr/bin/env python
#encoding: utf-8

import douban_robot

def generate_url():
    url_list = []
    start = 0
    perpage = 9
    total = 300
    baseurl = 'http://douban.fm/mine?type=liked&start='
    while start < total:
        url_list.append(baseurl+str(start))
        start = start + perpage
    return url_list
    
def fetch_urls():
    url_list = generate_url()
    robot = douban_robot.DoubanRobot()
    file_prefix = 'liked/liked_';
    for i in xrange(len(url_list)):
        print "[INFO]Fetch URL ", url_list[i]
        res, html = robot.get(url_list[i])
        if res:
            open(file_prefix + i +'.html', 'w').write(html)
        else:
            print "Error!"
            
if __name__ == "__main__":
    fetch_urls()