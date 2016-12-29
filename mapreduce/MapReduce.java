package ru.mipt;

import java.io.IOException;
import java.util.*;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

public class MapReduce {
	public static class Map extends Mapper<LongWritable, Text, Text, IntWritable> {
		private final String[] m_forbidden = { "Конец", "ПРИМЕЧАНИЯ", "=====" };
		public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
			String line = value.toString();
			String[] parts = line.split("\t");
			for(String str : m_forbidden) {
				if(parts[0].contains(str)) 
					return;
			}
			Text hero = new Text(parts[0]);
			StringTokenizer tokenizer = new StringTokenizer(parts[1]);
			context.write(hero, new IntWritable(tokenizer.countTokens()));
		}
	}

	public static class Reduce extends Reducer<Text, IntWritable, Text, Text> { 
		public void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
			int words = 0;
			int replics = 0;
			for (IntWritable val : values) {
				words += val.get();
				replics++;
			}
			double avg_words = words / replics;
			String res = new String(Integer.toString(words) + " [" + Integer.toString(replics)
					+ "] " + Double.toString(avg_words));
			context.write(key, new Text(res));
		}
	}

	public static void main(String[] args) throws Exception {
		Configuration conf = new Configuration();
		Job job = new Job(conf, "mapreduce");
		job.setJarByClass(MapReduce.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(IntWritable.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);
		job.setMapperClass(Map.class);
		job.setReducerClass(Reduce.class);
		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1]));
		job.waitForCompletion(true);
	}
}
