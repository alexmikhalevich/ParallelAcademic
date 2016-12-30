add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;

add file script.sh;

USE s49104;

SELECT TRANSFORM(ip, date, url, pagesize, statuscode, browser)
USING 'script.sh' FROM user_logs;
