package com.rex.utils;

import java.util.regex.PatternSyntaxException;

/**
 * Project Name:android
 * Author:CoorChice
 * Date:2017/7/20
 * Notes:
 */

public class SignedUtil {

  public static final String jBoolen = "Z";
  public static final String jByte = "B";
  public static final String jChar = "C";
  public static final String jShort = "S";
  public static final String jInt = "I";
  public static final String jLong = "J";
  public static final String jFloat = "F";
  public static final String jDouble = "D";
  public static final String jVoid = "V";
  public static final String jString = "Ljava/lang/String;";

  public static final String jBoolens = "[Z";
  public static final String jBytes = "[B";
  public static final String jChars = "[C";
  public static final String jShorts = "[S";
  public static final String jInts = "[I";
  public static final String jLongs = "[J";
  public static final String jFloats = "[F";
  public static final String jDoubles = "[D";
  public static final String jStrings = "[Ljava/lang/String;";

  public static final String jTwoDimensionalBytes = "[[B";

  public static final String jClassName(Class clazz) {
    if (clazz == null) {
      throw new NullPointerException("Class can't be null");
    }
    String className = clazz.getName();
    try {
      className = className.replaceAll("\\.", "/");
    } catch (PatternSyntaxException e) {
      e.printStackTrace();
    }
    return className;
  }


  public static final String jObject(Class clazz) {
    if (clazz == null) {
      throw new NullPointerException("Class can't be null");
    }
    return String.format("L%s;", jClassName(clazz));
  }

  public static final String jObjects(Class clazz) {
    if (clazz == null) {
      throw new NullPointerException("Class can't be null");
    }
    return String.format("[%s", jObject(clazz));
  }


  /**
   * 最后一个参数为返回值。
   * 无参返回"()V"
   */
  public static final String getMethodSigned(String... args) {
    if (args == null || args.length == 0) {
      return "()V";
    }
    StringBuffer sb = new StringBuffer();
    sb.append("(");
    int length = args.length;
    for (int i = 0; i < length; i++) {
      if (i == length - 1) {
        sb.append(")");

      }
      sb.append(args[i]);
    }
    return sb.toString();
  }
}
