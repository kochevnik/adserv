#!/usr/bin/python
import sys
import os
from optparse import OptionParser
import psycopg2
import psycopg2.extras

def get_template(cursor, options):
	cursor.execute("SELECT template FROM templates WHERE place_id=%s AND inst_id=%s AND iframe=%s;", (options.place_id, options.inst_id, options.iframe))
	template = cursor.fetchone()[0]
	if None != template:
		f = open(options.file, "w")
		f.write(str(template))
		f.close()


def put_template(cursor, options):
	#st = os.stat(filename)
	f = open(options.file, 'r')
	#template = f.read(st.st_size)
	template = f.read()
	f.close()
	cursor.execute("INSERT INTO templates (place_id, inst_id, iframe, template) VALUES (%s, %s, %s, %s);", (options.place_id, options.inst_id, options.iframe, psycopg2.Binary(template)))


def get_banner(cursor, options):
	cursor.execute("SELECT banner FROM banners WHERE place_id=%s AND banner_id=%s", (options.place_id, options.banner_id))
	banner = cursor.fetchone()[0]
	if None != banner:
		f = open(options.file, "w")
		f.write(str(banner))
		f.close()


def put_banner(cursor, options):
	f = open(options.file, 'r')
	banner = f.read()
	f.close()
	cursor.execute("INSERT INTO banners (place_id, banner_id, banner) VALUES (%s, %s, %s);", (options.place_id, options.banner_id, psycopg2.Binary(banner)))


def main():
	parser = OptionParser()
	parser.add_option("-c", "--cmd", dest="cmd", help="command (get_template | put_template | get_banner | put_banner)")
	parser.add_option("-f", "--file", dest="file", help="file to use", metavar="FILE")
	parser.add_option("--place_id", dest="place_id")
	parser.add_option("--inst_id", dest="inst_id")
	parser.add_option("--iframe", dest="iframe")
	parser.add_option("--banner_id", dest="banner_id")
	(options, args) = parser.parse_args()

	conn_string = "host='localhost' password='123'"
	conn = psycopg2.connect(conn_string)
	conn.set_isolation_level(psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT)
	cursor = conn.cursor()
	if "get_template" == options.cmd:
		get_template(cursor, options)
	elif "put_template" == options.cmd:
		put_template(cursor, options)
	elif "get_banner" == options.cmd:
		get_banner(cursor, options)
	elif "put_banner" == options.cmd:
		put_banner(cursor, options)
	else:
		print "unexpected command %s" % options.cmd
	conn.close()
	return 0


if __name__ == "__main__":
	sys.exit(main())

