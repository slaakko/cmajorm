@echo off
bin\release\xpq "/" test.xml > q1.txt
bin\release\xpq "/foo" test.xml > q2.txt
bin\release\xpq "/test" test.xml > q3.txt
bin\release\xpq "/test/processing-instruction()" test.xml > q4.txt
bin\release\xpq "/test/alpha" test.xml > q5.txt
bin\release\xpq "/test/alpha[@a=\"1\"]" test.xml > q6.txt
bin\release\xpq "/test/alpha[@a!=\"1\" and @b!=\"2\"]" test.xml > q7.txt
bin\release\xpq "/test/alpha[@c=\"3\"]" test.xml > q8.txt
bin\release\xpq "/test/alpha/beta/gamma[text()=\"text1\"]" test.xml > q9.txt
bin\release\xpq "/test/alpha/beta/gamma/text()" test.xml > q10.txt
bin\release\xpq "/test/alpha/beta/gamma[text()=\"text2\"]/node()" test.xml > q11.txt
bin\release\xpq "/test/alpha/beta[@name=\"foo\"]/delta/comment()" test.xml > q12.txt
bin\release\xpq "string(/test/alpha[@c=\"3\"])" test.xml > q13.txt
bin\release\xpq "//parameter" System.Json.xml > q14.txt
bin\release\xpq "//parameter/type[@ref=\"type.5822889\"]/.." System.Json.xml > q15.txt