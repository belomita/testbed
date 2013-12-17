#!/usr/bin/env python
#encoding: utf-8

import douban_robot
import time

def generate_url():
    url_list = []
    start = 0
    perpage = 9
    total = 326
    baseurl = 'http://douban.fm/mine?type=liked'
    url_list.append(baseurl)
    baseurl = baseurl + "&start="
    while start < total:
        url_list.append(baseurl+str(start))
        start = start + perpage
    return url_list
    
def fetch_urls():
    url_list = generate_url()
    robot = douban_robot.DoubanRobot()
    file_prefix = 'liked/liked_';
    for i in xrange(len(url_list)):
        print "[INFO]Fetch ", i, "th URL ", url_list[i]
        read_count = 1024 * 10
        if i == 0:
            read_count = 100
        res, html = robot.get(url_list[i], read_count)
        if res:
            open(file_prefix + str(i) +'.html', 'w').write(html)
        else:
            if not robot.is_login:
                return
        time.sleep(0.5)

if __name__ == "__main__":
    fetch_urls()