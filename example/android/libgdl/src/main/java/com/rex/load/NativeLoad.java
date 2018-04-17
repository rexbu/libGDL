package com.rex.load;

/**
* @date 16/8/12 2:09 PM
* @author VisionIn
*/
public class NativeLoad {
  static {
    // System.loadLibrary("zr")
    System.loadLibrary("native");
  }

  public long so;

  public static native long loadSo(String so);

  public static native int registerJNIMethod(long so, String className, String funcName, String signature);

  /*
 * 类型 相应的签名
 * boolean Z
 * byte B
 * char C
 * short S
 * int I
 * long J
 * float F
 * double D
 * void V
 * object L用/分隔包的完整类名： LJava/lang/String;
 * Array [签名 [I [LJava/lang/Object;
 * Method (参数1类型签名 参数2类型签名···)返回值类型签名
 * 复制代码
 * 特别注意：Object后面一定有分号（；）结束的,多个对象参数中间也用分号(;)来分隔
 * 例子：
 * 方法签名
 * void f1() ()V
 * int f2(int, long) (IJ)I
 * boolean f3(int[]) ([I)B
 * double f4(String, int) (LJava/lang/String;I)D
 * void f5(int, String [], char) (I[LJava/lang/String;C)V
 */

}
