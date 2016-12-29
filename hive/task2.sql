add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;

USE s49104;

SELECT date, COUNT(date) as amount FROM user_logs 
GROUP BY date
ORDER BY amount;
