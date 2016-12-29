package com.lab4udf;

import org.apache.hadoop.hive.ql.exec.Description;
import org.apache.hadoop.hive.ql.exec.UDF;


@Description(
        name = "ReverseUDF",
        value = "Returns reversed string",
        extended = "none"
)
public class ReverseUDF extends UDF {
    public String evaluate(String str){ 
	    return new StringBuilder(str).reverse().toString();
    }
}
