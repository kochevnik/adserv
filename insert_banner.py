#!/usr/bin/python
# -*- coding: ascii -*-
import sys
import os
import psycopg2

def main():
	argv = sys.argv
	if len(argv) != 2:
		print "usage: %s file" % argv[0]
		return 1

	f = open(argv[1], 'r')
	banner = f.read()
	f.close()

	conn_string = "host='localhost' password='123'"
	conn = psycopg2.connect(conn_string)
	conn.set_isolation_level(psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT)
	cursor = conn.cursor()
	cursor.execute("INSERT INTO banner (id, uid, targetUrl, contentType, contentTypeId, content, width, height) VALUES (%s, %s, %s, %s, %s, %s, %s, %s);", (1, 123456, 'http://www.google.com', 'image/gif', 0, psycopg2.Binary(banner), 468, 60))
	conn.close()
	return 0


if __name__ == "__main__":
	sys.exit(main())

