@echo off
cmdoc --verbose --optimize cmdoc/code.cmdoc.xml
cmdoc --verbose --optimize compression/cmdoc/compression.cmdoc.xml
cmdoc --verbose --optimize http/cmdoc/httpClients.cmdoc.xml
cmdoc --verbose --optimize json/cmdoc/json.cmdoc.xml
cmdoc --verbose --optimize solution/cmdoc/solution.cmdoc.xml
cmdoc --verbose --optimize socket/cmdoc/socketClients.cmdoc.xml
cmdoc --verbose --optimize threading/cmdoc/threading.cmdoc.xml
cmdoc --verbose --optimize xml/cmdoc/xml.cmdoc.xml
