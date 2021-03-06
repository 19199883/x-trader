#!/bin/sh

if [ "${srcdir}" = "." ]; then
    lsrc=""
else
    lsrc="${srcdir}/"
fi

expected="<?xml version=\"1.0\"?>
<?xml-stylesheet type=\"text/xsl\" href=\"http://check.sourceforge.net/xml/check_unittest.xslt\"?>
<testsuites xmlns=\"http://check.sourceforge.net/ns\">
  <suite>
    <title>S1</title>
    <test result=\"success\">
      <fn>ex_xml_output.c:10</fn>
      <id>test_pass</id>
      <iteration>0</iteration>
      <description>Core</description>
      <message>Passed</message>
    </test>
    <test result=\"failure\">
      <fn>ex_xml_output.c:16</fn>
      <id>test_fail</id>
      <iteration>0</iteration>
      <description>Core</description>
      <message>Failure</message>
    </test>
    <test result=\"error\">
      <fn>ex_xml_output.c:20</fn>
      <id>test_exit</id>
      <iteration>0</iteration>
      <description>Core</description>
      <message>Early exit with return value 1</message>
    </test>
  </suite>
  <suite>
    <title>S2</title>
    <test result=\"success\">
      <fn>ex_xml_output.c:28</fn>
      <id>test_pass2</id>
      <iteration>0</iteration>
      <description>Core</description>
      <message>Passed</message>
    </test>
    <test result=\"failure\">
      <fn>ex_xml_output.c:34</fn>
      <id>test_loop</id>
      <iteration>0</iteration>
      <description>Core</description>
      <message>Iteration 0 failed</message>
    </test>
    <test result=\"success\">
      <fn>ex_xml_output.c:34</fn>
      <id>test_loop</id>
      <iteration>1</iteration>
      <description>Core</description>
      <message>Passed</message>
    </test>
    <test result=\"failure\">
      <fn>ex_xml_output.c:34</fn>
      <id>test_loop</id>
      <iteration>2</iteration>
      <description>Core</description>
      <message>Iteration 2 failed</message>
    </test>
  </suite>
  <suite>
    <title>XML escape &quot; &apos; &lt; &gt; &amp; tests</title>
    <test result=\"failure\">
      <fn>ex_xml_output.c:40</fn>
      <id>test_xml_esc_fail_msg</id>
      <iteration>0</iteration>
      <description>description &quot; &apos; &lt; &gt; &amp;</description>
      <message>fail &quot; &apos; &lt; &gt; &amp; message</message>
    </test>
  </suite>
</testsuites>"

./ex_xml_output > /dev/null
actual=`cat test.log.xml | grep -v \<duration\> | grep -v \<datetime\> | grep -v \<path\>`
if [ x"${expected}" != x"${actual}" ]; then
    echo "Problem with ex_xml_output ${3}";
    echo "Expected:";
    echo "${expected}";
    echo "Got:";
    echo "${actual}";
    exit 1;
fi

duration_count=`grep -c \<duration\> test.log.xml`
if [ x"${duration_count}" != x9 ]; then
    echo "Wrong number of <duration> elements in test.log.xml";
    exit 1;
fi

duration_error=`cat test.log.xml | grep \<duration\> | grep -c "\-1\.000000"`
if [ x"${duration_error}" != x5 ]; then
    echo "Wrong format for (or number of) error <duration> elements in test.log.xml";
    exit 1;
fi

exit 0
