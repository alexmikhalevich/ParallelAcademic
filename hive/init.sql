add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;

drop database if exists s49104;
create database s49104 location '/user/s49104/metastore';
