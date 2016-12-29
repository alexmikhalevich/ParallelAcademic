add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;

USE s49104;

SELECT ip_data.region, user_data.sex, COUNT(1) 
FROM user_logs
INNER JOIN ip_data ON user_logs.ip = ip_data.ip 
INNER JOIN user_data ON user_data.ip = user_logs.ip
GROUP BY ip_data.region, user_data.sex;
