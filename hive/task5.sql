add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;

USE s49104;

add jar jar/Reverser.jar;
CREATE TEMPORARY FUNCTION reverse AS 'com.lab4udf.ReverseUDF';
SELECT reverse(ip) FROM user_logs;
