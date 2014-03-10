from mod_python import apache
from mod_python import util
import datetime,os, codecs

def handler(req):
        req.log_error('handler')
        req.content_type = 'text/html'
        req.send_http_header()
        #req.write('<html><head><title>Testing mod_python</title></head><body>')
        #req.write('%s'%util.FieldStorage(req)["file"])
        try:
                date =  datetime.datetime.now().strftime("%Y %m %d %H:%M:%S")
                ip = req.connection.remote_ip
                name = u"%s/%s %s.tar.gz"%(os.path.dirname(os.path.abspath(__file__)),date,ip)
                f = open(name.encode(sys.getfilesystemencoding()),"w")
                f.write(util.FieldStorage(req)["file"])
                f.close()
                req.write('OK')
        except Exception as e:
                req.write('%s'%e)
        #req.write('</body></html>')
        return apache.OK

