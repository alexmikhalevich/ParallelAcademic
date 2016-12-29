add jar /opt/cloudera/parcels/CDH/jars/hive-contrib-1.1.0-cdh5.7.0.jar;

USE s49104;

SELECT ip_regions.region 
FROM (
	SELECT AVG(tmp_table.hits_num) as avg
	FROM (
		SELECT region, SUM(ip_list.ip_amount) as hits_num
		FROM ip_data, 
		(
			SELECT ip, COUNT(ip) as ip_amount FROM user_logs
			GROUP BY ip
		) as ip_list
		WHERE ip_data.ip == ip_list.ip
		GROUP BY region
	) as tmp_table
) as avg_table,
(
	SELECT region, SUM(ip_list.ip_amount) as hits_num
	FROM ip_data, 
	(
		SELECT ip, COUNT(ip) as ip_amount FROM user_logs
		GROUP BY ip
	) as ip_list
	WHERE ip_data.ip == ip_list.ip
	GROUP BY region
) as ip_regions
WHERE ip_regions.hits_num > avg_table.avg;
