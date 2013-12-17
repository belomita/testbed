#!/usr/bin/env python

import os, sys, os.path, json

class Song:
    def __init__(self, name, album, singer):
        self.name = name
        self.album = album
        self.singer = singer
    def __hash__(self):
        return hash(''.join([self.name,self.album,self.singer]))

    def __cmp__(self, other):
        return cmp(''.join([self.name,self.album,self.singer]), 
                ''.join([other.name , other.album , other.singer]))

    def println(self):
        print '\t'.join([self.name, self.album, self.singer])
    def as_dict(self):
        return {'name':self.name, 'album':self.album, 'singer':self.singer}

# find the table section: start tag "<table>", end tag "</table>"
# @param content - string of html file
# @return the start and end index
def find_table(content):
    start = content.find('<table')
    if start < 0:
        return -1, -1
    end = content.find('</table>', start) + len('</table>')
    if start > end:
        return -1, -1
    return start, end

# parse out song list from the html table
# @param content - string of html file
# @param start - table tag start index
# @param end - table tag end index
# @return songs list
def parse_table(content, start, end):
    tag_start = False
    tag_end = False
    front = start
    back = start 
    buf = []
    while front <= end:
        if content[front] == '<':
            tag_start = True
            if tag_end == True:
                tag_end = False
                substr = content[back:front]
                if len(substr) > 0 and not substr.isspace():
                    buf.append(substr.strip())
                continue
        if content[front] == '>':
            tag_end = True
            tag_start = False
            back = front + 1
        front = front + 1
    buf = buf[3:]
    songs = []
    for i in xrange(0, len(buf), 3):
        songs.append(Song(buf[i], buf[i+1], buf[i+2]))
    return songs

# parse douban like html file to parse out songs I like
# songs are paged in multiple web pages
def parse_file(filepath):
    fd = open(filepath)
    content = fd.read()
    start, end = find_table(content)
    if start >= 0 and end > start:
        songs = parse_table(content, start, end)
        return songs
    return []

# parse all songs info out in directory
def parse_directory(dirpath):
    songs = []
    os.chdir(dirpath)
    #print os.path.abspath('.')
    for file in os.listdir('.'):
        songs = songs + parse_file(file)
    # remove duplicates
    songs = list(set(songs))
    return songs

# export as json to out_stream
def export_songs_json(songs, out_stream):
    song_dict_list = [s.as_dict() for s in songs]
    out_stream.write('song_list = ')
    json.dump(song_dict_list, out_stream, sort_keys=True, indent=4)

def statistics(songs):
    stats = {}
    stats['total']  = len(songs)
    stats['album']  = {}
    stats['singer'] = {}
    
    # album group
    grouped_map = stats['album']
    for item in songs:
        if item.album not in grouped_map:
            grouped_map[item.album] = {'total' : 1, 'list' : [item.name]}
        else:
            grouped_map[item.album]['total'] = grouped_map[item.album]['total'] + 1
            grouped_map[item.album]['list'].append(item.name)
    
    # singer group
    grouped_map = stats['singer']
    for item in songs:
        if item.singer not in grouped_map:
            grouped_map[item.singer] = {'total' : 1, 'list' : [item.name]}
        else:
            grouped_map[item.singer]['total'] = grouped_map[item.singer]['total'] + 1
            grouped_map[item.singer]['list'].append(item.name)
    return stats

class Stat:
    def __init__(self, name):
        self.name = name
        self.count = 0
        self.songs = None

    def set_songs(self, songs):
        self.count = len(songs)
        self.songs = songs;
        
    def as_dict(self):
        return {'name':self.name, 'count':self.count, 'songs':self.songs}
        
def statistics2(songs):
    stats = {}
    stats['total']  = len(songs)
    
    res = statistics(songs)
    for kind in ['album', 'singer']:
        ins_list = []
        grouped_map = res[kind]
        for k,v in grouped_map.items():
            ins = Stat(k)
            ins.set_songs(v['list'])
            ins_list.append(ins)
        ins_list.sort(cmp=lambda x, y: x.count-y.count, reverse=True)
        stats[kind] = [i.as_dict() for i in ins_list]
    return stats

def export_stats_json(stats, out_stream):
    out_stream.write('stats = ')
    json.dump(stats, out_stream, sort_keys=True, indent=4)

if __name__ == '__main__':
    page_dir = 'pages'   # default pages directory
    song_file = sys.stdout  # default songs output file
    stats_file = sys.stdout # default stats output file
    if len(sys.argv) >= 2:
        page_dir = sys.argv[1]
    if len(sys.argv) >= 3:
        song_file = open(sys.argv[2], 'w')
    if len(sys.argv) >= 4:
        stats_file = open(sys.argv[3], 'w')
        
    songs = parse_directory(page_dir)
    stats = statistics2(songs)
    
    export_songs_json(songs, song_file)
    export_stats_json(stats, stats_file)
    song_file.flush()
    
    if song_file is not sys.stdout:
        song_file.close()
    if stats_file is not sys.stdout:
        stats_file.close()