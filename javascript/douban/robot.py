#encoding: utf-8

import re, urllib, urllib2, cookielib

class DoubanRobot:
    email = ''
    password = ''
    login_path = 'https://www.douban.com/accounts/login'
    
    def __init__(self, debuglevel=1):
        self.cookiejar = cookielib.LWPCookieJar()
        self.opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(self.cookiejar))
        urllib2.install_opener(self.opener)
        self.opener.addheaders = [('User-agent', 'Opera/9.23')]
        
        self.debuglevel = debuglevel
        self.is_login = False
    
    # login 
    # return False if failed otherwise return True
    def login(self):
        if self.debuglevel > 0:
            print '[INFO]Log into website:', self.login_path
        post_data = urllib.urlencode({
            'form_email' : self.email,
            'form_password' : self.password,
            'remember' : 'on',
            })
        
        # send http request
        request = urllib2.Request(self.login_path, post_data)
        html = self.opener.open(request).read(100)
        #if self.debuglevel > 0:
        #    print '[DEBUG]' + re.findall('<h1>(.*)</h1>', html, re.S)[0]#.decode('utf8')
        
        get_url = self.opener.open(request).geturl()
        if get_url == 'http://www.douban.com/':
            self.cookiejar.save('douban.cookie')
            if self.debuglevel > 0:
                print '[INFO]Login Sucess'
            self.is_login = True
        else:
            if self.debuglevel > 0:
                print '[ERROR]Login error, GET URL:', get_url
            self.is_login = False
        return self.is_login
    
    # open url and get response body
    # return (False, None) if failed
    # return (True, body) if success
    def get(self, url, read_amount):
        if (not self.is_login) and (not self.login()):
            if self.debuglevel > 0:
                print '[ERROR]Can not fetch url due to login failed!'
            return False, None
        request = urllib2.Request(url)
        html = urllib2.urlopen(request).read(read_amount)
        return True, html    
        