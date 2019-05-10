// SIMD-ONLY0-NOT: {{__kmpc|__tgt}}

// expected-no-diagnostics
#ifndef HEADER
#define HEADER

///==========================================================================///
// RUN: %clang_cc1 -DCK0 -verify -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix CK0 --check-prefix CK0-64 %s
// RUN: %clang_cc1 -DCK0 -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -std=c++11 -triple powerpc64le-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -DCK0 -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix CK0 --check-prefix CK0-64 %s
// RUN: %clang_cc1 -DCK0 -verify -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix CK0 --check-prefix CK0-32 %s
// RUN: %clang_cc1 -DCK0 -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -std=c++11 -triple i386-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -DCK0 -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix CK0 --check-prefix CK0-32 %s

// RUN: %clang_cc1 -DCK0 -verify -fopenmp-simd -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix SIMD-ONLY0 %s
// RUN: %clang_cc1 -DCK0 -fopenmp-simd -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -std=c++11 -triple powerpc64le-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -DCK0 -fopenmp-simd -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix SIMD-ONLY0 %s
// RUN: %clang_cc1 -DCK0 -verify -fopenmp-simd -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix SIMD-ONLY0 %s
// RUN: %clang_cc1 -DCK0 -fopenmp-simd -fopenmp-targets=i386-pc-linux-gnu -x c++ -std=c++11 -triple i386-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -DCK0 -fopenmp-simd -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix SIMD-ONLY0 %s

#ifdef CK0

// CK0-LABEL: @.__omp_offloading_{{.*}}foo{{.*}}.region_id = weak constant i8 0
// CK0-64: [[SIZES:@.+]] = {{.+}}constant [1 x i[[sz:64|32]]] [i64 16]
// CK0-32: [[SIZES:@.+]] = {{.+}}constant [1 x i[[sz:64|32]]] [i32 8]
// CK0: [[TYPES:@.+]] = {{.+}}constant [1 x i64] [i64 35]
// CK0-64: [[TSIZES:@.+]] = {{.+}}constant [1 x i[[sz]]] [i[[sz]] 16]
// CK0-32: [[TSIZES:@.+]] = {{.+}}constant [1 x i[[sz]]] [i[[sz]] 8]
// CK0: [[TTYPES:@.+]] = {{.+}}constant [1 x i64] [i64 33]
// CK0-64: [[FSIZES:@.+]] = {{.+}}constant [1 x i[[sz]]] [i[[sz]] 16]
// CK0-32: [[FSIZES:@.+]] = {{.+}}constant [1 x i[[sz]]] [i[[sz]] 8]
// CK0: [[FTYPES:@.+]] = {{.+}}constant [1 x i64] [i64 34]

class C {
public:
  int a;
  double *b;
};

#pragma omp declare mapper(id: C s) map(s.a, s.b[0:2])

// Synchronous version of mapper function.
// CK0-LABEL: define {{.*}}i32 @.omp_mapper.class_C.id{{.*}}(i8*, i8*, i8*, i{{64|32}}, i64)
// CK0-DAG: store i8* %0, i64* [[HANDLEADDR:%[^,]+]]
// CK0-DAG: store i8* %1, i8** [[BPTRADDR:%[^,]+]]
// CK0-DAG: store i8* %2, i8** [[VPTRADDR:%[^,]+]]
// CK0-DAG: store i[[sz:64|32]] %3, i{{64|32}}* [[SIZEADDR:%[^,]+]]
// CK0-DAG: store i64 %4, i64* [[TYPEADDR:%[^,]+]]
// CK0-DAG: [[SIZE:%.+]] = load i[[sz]], i[[sz]]* [[SIZEADDR]]
// CK0-DAG: [[TYPE:%.+]] = load i64, i64* [[TYPEADDR]]
// CK0-DAG: [[HANDLE:%.+]] = load i8*, i8** [[HANDLEADDR]]
// CK0-DAG: [[PTRBEGIN:%.+]] = bitcast i8** [[VPTRADDR]] to %class.C**
// CK0-DAG: [[PTREND:%.+]] = getelementptr %class.C*, %class.C** [[PTRBEGIN]], i[[sz]] [[SIZE]]
// CK0: [[ISARRAY:%.+]] = icmp sge i[[sz]] [[SIZE]], 1
// CK0: br i1 [[ISARRAY]], label %[[INITEVALDEL:[^,]+]], label %[[LHEAD:[^,]+]]

// CK0: [[INITEVALDEL]]
// CK0: [[TYPEDEL:%.+]] = and i64 [[TYPE]], 8
// CK0: [[ISNOTDEL:%.+]] = icmp eq i64 [[TYPEDEL]], 0
// CK0: br i1 [[ISNOTDEL]], label %[[INIT:[^,]+]], label %[[LHEAD:[^,]+]]
// CK0: [[INIT]]
// CK0-64: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 16
// CK0-32: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 8
// CK0-DAG: [[ITYPE:%.+]] = and i64 [[TYPE]], -4
// CK0: call void @__kmpc_push_mapper_component(i8* [[HANDLE]], i8* [[BPTRADDR]], i8* [[VPTRADDR]], i[[sz]] [[ARRSIZE]], i64 [[ITYPE]])
// CK0: br label %[[LHEAD:[^,]+]]

// CK0: [[LHEAD]]
// CK0: [[ISEMPTY:%.+]] = icmp eq %class.C** [[PTRBEGIN]], [[PTREND]]
// CK0: br i1 [[ISEMPTY]], label %[[DONE]], label %[[LBODY:[^,]+]]
// CK0: [[LBODY]]
// CK0: [[PTR:%.+]] = phi %class.C** [ [[PTRBEGIN]], %[[LHEAD]] ], [ [[PTRNEXT:%.+]], %[[LCORRECT:[^,]+]] ]
// CK0: [[OBJ:%.+]] = load %class.C*, %class.C** [[PTR]]
// CK0-DAG: [[ABEGIN:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 0
// CK0-DAG: [[BBEGIN:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 1
// CK0-DAG: [[BBEGIN2:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 1
// CK0-DAG: [[BARRBEGIN:%.+]] = load double*, double** [[BBEGIN2]]
// CK0-DAG: [[BARRBEGINGEP:%.+]] = getelementptr inbounds double, double* [[BARRBEGIN]], i[[sz]] 0
// CK0-DAG: [[BEND:%.+]] = getelementptr double*, double** [[BBEGIN]], i32 1
// CK0-DAG: [[ABEGINV:%.+]] = bitcast i32* [[ABEGIN]] to i8*
// CK0-DAG: [[BENDV:%.+]] = bitcast double** [[BEND]] to i8*
// CK0-DAG: [[ABEGINI:%.+]] = ptrtoint i8* [[ABEGINV]] to i64
// CK0-DAG: [[BENDI:%.+]] = ptrtoint i8* [[BENDV]] to i64
// CK0-DAG: [[CSIZE:%.+]] = sub i64 [[BENDI]], [[ABEGINI]]
// CK0-DAG: [[CUSIZE:%.+]] = sdiv exact i64 [[CSIZE]], ptrtoint (i8* getelementptr (i8, i8* null, i32 1) to i64)
// CK0-DAG: [[BPTRADDR0BC:%.+]] = bitcast %class.C* [[OBJ]] to i8*
// CK0-DAG: [[PTRADDR0BC:%.+]] = bitcast i32* [[ABEGIN]] to i8*
// CK0-DAG: [[TYPETF:%.+]] = and i64 [[TYPE]], 3
// CK0-DAG: [[ISALLOC:%.+]] = icmp eq i64 [[TYPETF]], 0
// CK0-DAG: br i1 [[ISALLOC]], label %[[ALLOC:[^,]+]], label %[[ALLOCELSE:[^,]+]]
// CK0-DAG: [[ALLOC]]
// CK0-DAG: br label %[[TYEND:[^,]+]]
// CK0-DAG: [[ALLOCELSE]]
// CK0-DAG: [[ISTO:%.+]] = icmp eq i64 [[TYPETF]], 1
// CK0-DAG: br i1 [[ISTO]], label %[[TO:[^,]+]], label %[[TOELSE:[^,]+]]
// CK0-DAG: [[TO]]
// CK0-DAG: br label %[[TYEND]]
// CK0-DAG: [[TOELSE]]
// CK0-DAG: [[ISFROM:%.+]] = icmp eq i64 [[TYPETF]], 2
// CK0-DAG: br i1 [[ISFROM]], label %[[FROM:[^,]+]], label %[[TYEND]]
// CK0-DAG: [[FROM]]
// CK0-DAG: br label %[[TYEND]]
// CK0-DAG: [[TYEND]]
// CK0-DAG: [[PHITYPE0:%.+]] = phi i64 [ 32, %[[ALLOC]] ], [ 32, %[[TO]] ], [ 32, %[[FROM]] ], [ 32, %[[TOELSE]] ]
// CK0: call void @__kmpc_push_mapper_component(i8* [[HANDLE]], i8* [[BPTRADDR0BC]], i8* [[PTRADDR0BC]], i[[sz]] [[CUSIZE]], i64 [[PHITYPE0]])
// 0x1,000,000,000; 0x1,000,000,001; 0x1,000,000,002; 0x1,000,000,003
// CK0-DAG: [[TYPE1:%.+]] = phi i64 [ 281474976710656, %[[ALLOC]] ], [ 281474976710657, %[[TO]] ], [ 281474976710658, %[[FROM]] ], [ 281474976710659, %[[TOELSE]] ]
// 0x1,000,000,010; 0x1,000,000,011; 0x1,000,000,012; 0x1,000,000,013
// CK0-DAG: [[TYPE2:%.+]] = phi i64 [ 281474976710672, %[[ALLOC]] ], [ 281474976710673, %[[TO]] ], [ 281474976710674, %[[FROM]] ], [ 281474976710675, %[[TOELSE]] ]
// CK0-DAG: store i64 [[TYPE2]], i64* [[TYPEADDR2]]
// CK0-DAG: [[ARGBPTR:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[LBPTRADDR]], i32 0, i32 0
// CK0-DAG: [[ARGPTR:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[PTRADDR]], i32 0, i32 0
// CK0-DAG: [[ARGSIZE:%.+]] = getelementptr inbounds [3 x i[[sz]]], [3 x i[[sz]]]* [[SIZEADDR]], i32 0, i32 0
// CK0-DAG: [[ARGTYPE:%.+]] = getelementptr inbounds [3 x i64], [3 x i64]* [[TYPEADDR]], i32 0, i32 0
// CK0: [[RES:%.+]] = call i32 @__tgt_target_data_mapper(i64 [[HANDLE]], i32 3, i8** [[ARGBPTR]], i8** [[ARGPTR]], i[[sz]]* [[ARGSIZE]], i64* [[ARGTYPE]], i8** null)
// CK0: [[ISERR:%.+]] = icmp ne i32 [[RES]], 0
// CK0: br i1 [[ISERR]], label %[[LERR:[^,]+]], label %[[LCORRECT]]
// CK0: [[LERR]]
// CK0: store i32 [[RES]], i32* %retval
// CK0: br label %[[DONE]]
// CK0: [[LCORRECT]]
// CK0: [[PTRNEXT]] = getelementptr %class.C*, %class.C** [[PTR]], i32 1
// CK0: [[ISDONE:%.+]] = icmp eq %class.C** [[PTRNEXT]], [[PTREND]]
// CK0: br i1 [[ISDONE]], label %[[LEXIT:[^,]+]], label %[[LBODY]]

// CK0: [[LEXIT]]
// CK0: br i1 [[ISARRAY]], label %[[EVALDEL:[^,]+]], label %[[DONE]]
// CK0: [[EVALDEL]]
// CK0: [[TYPEDEL:%.+]] = and i64 [[TYPE]], 8
// CK0: [[ISDEL:%.+]] = icmp ne i64 [[TYPEDEL]], 0
// CK0: br i1 [[ISDEL]], label %[[DEL:[^,]+]], label %[[DONE]]
// CK0: [[DEL]]
// CK0-64: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 16
// CK0-32: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 8
// CK0-DAG: store i[[sz]] [[ARRSIZE]], i[[sz]]* [[DSIZEADDR:%[^,]+]]
// CK0-DAG: [[DSIZEADDR]] = getelementptr inbounds [1 x i[[sz]]], [1 x i[[sz]]]* [[DSIZE:%.+]], i32 0, i32 0
// CK0-DAG: [[DTYPE:%.+]] = and i64 [[TYPE]], -4
// CK0-DAG: store i64 [[DTYPE]], i64* [[DTYPEADDR:%[^,]+]]
// CK0-DAG: [[DTYPEADDR]] = getelementptr inbounds [1 x i64], [1 x i64]* [[DTYPE:%.+]], i32 0, i32 0
// CK0: [[DRES:%.+]] = call i32 @__tgt_target_data_mapper(i64 [[HANDLE]], i32 1, i8** [[BPTRADDR]], i8** [[VPTRADDR]], i[[sz]]* [[DSIZEADDR]], i64* [[DTYPEADDR]], i8** null)
// CK0: [[ISDELERR:%.+]] = icmp ne i32 [[DRES]], 0
// CK0: br i1 [[ISDELERR]], label %[[DELERR:[^,]+]], label %[[DONE]]
// CK0: [[DELERR]]
// CK0: store i32 [[DRES]], i32* %retval
// CK0: br label %[[DONE]]
// CK0: [[DONE]]
// CK0: [[RET:%.+]] = load i32, i32* %retval
// CK0: ret i32 [[RET]]


// CK0-LABEL: define {{.*}}void @{{.*}}foo{{.*}}
void foo(int a){
  int i = a;
  C c;
  c.a = a;

  // CK0-DAG: call i32 @__tgt_target(i64 {{.+}}, i8* {{.+}}, i32 1, i8** [[BPGEP:%[0-9]+]], i8** [[PGEP:%[0-9]+]], {{.+}}[[SIZES]]{{.+}}, {{.+}}[[TYPES]]{{.+}})
  // CK0-DAG: [[BPGEP]] = getelementptr inbounds {{.+}}[[BPS:%[^,]+]], i32 0, i32 0
  // CK0-DAG: [[PGEP]] = getelementptr inbounds {{.+}}[[PS:%[^,]+]], i32 0, i32 0
  // CK0-DAG: [[BP1:%.+]] = getelementptr inbounds {{.+}}[[BPS]], i32 0, i32 0
  // CK0-DAG: [[P1:%.+]] = getelementptr inbounds {{.+}}[[PS]], i32 0, i32 0
  // CK0-DAG: [[CBP1:%.+]] = bitcast i8** [[BP1]] to %class.C**
  // CK0-DAG: [[CP1:%.+]] = bitcast i8** [[P1]] to %class.C**
  // CK0-DAG: store %class.C* [[VAL:%[^,]+]], %class.C** [[CBP1]]
  // CK0-DAG: store %class.C* [[VAL]], %class.C** [[CP1]]
  // CK0: call void [[KERNEL:@.+]](%class.C* [[VAL]])
  #pragma omp target map(mapper(id),tofrom: c)
  {
   ++c.a;
  }

  // CK0-DAG: call void @__tgt_target_data_update(i64 -1, i32 1, i8** [[TGEPBP:%.+]], i8** [[TGEPP:%.+]], i[[sz]]* getelementptr {{.+}}[1 x i[[sz]]]* [[TSIZES]], i32 0, i32 0), {{.+}}getelementptr {{.+}}[1 x i64]* [[TTYPES]]{{.+}})
  // CK0-DAG: [[TGEPBP]] = getelementptr inbounds {{.+}}[[TBP:%[^,]+]], i{{.+}} 0, i{{.+}} 0
  // CK0-DAG: [[TGEPP]] = getelementptr inbounds {{.+}}[[TP:%[^,]+]], i{{.+}} 0, i{{.+}} 0
  // CK0-DAG: [[TBP0:%.+]] = getelementptr inbounds {{.+}}[[TBP]], i{{.+}} 0, i{{.+}} 0
  // CK0-DAG: [[TP0:%.+]] = getelementptr inbounds {{.+}}[[TP]], i{{.+}} 0, i{{.+}} 0
  // CK0-DAG: [[TCBP0:%.+]] = bitcast i8** [[TBP0]] to %class.C**
  // CK0-DAG: [[TCP0:%.+]] = bitcast i8** [[TP0]] to %class.C**
  // CK0-DAG: store %class.C* [[VAL]], %class.C** [[TCBP0]]
  // CK0-DAG: store %class.C* [[VAL]], %class.C** [[TCP0]]
  #pragma omp target update to(mapper(id): c)

  // CK0-DAG: call void @__tgt_target_data_update(i64 -1, i32 1, i8** [[FGEPBP:%.+]], i8** [[FGEPP:%.+]], i[[sz]]* getelementptr {{.+}}[1 x i[[sz]]]* [[FSIZES]], i32 0, i32 0), {{.+}}getelementptr {{.+}}[1 x i64]* [[FTYPES]]{{.+}})
  // CK0-DAG: [[FGEPBP]] = getelementptr inbounds {{.+}}[[FBP:%[^,]+]], i{{.+}} 0, i{{.+}} 0
  // CK0-DAG: [[FGEPP]] = getelementptr inbounds {{.+}}[[FP:%[^,]+]], i{{.+}} 0, i{{.+}} 0
  // CK0-DAG: [[FBP0:%.+]] = getelementptr inbounds {{.+}}[[FBP]], i{{.+}} 0, i{{.+}} 0
  // CK0-DAG: [[FP0:%.+]] = getelementptr inbounds {{.+}}[[FP]], i{{.+}} 0, i{{.+}} 0
  // CK0-DAG: [[FCBP0:%.+]] = bitcast i8** [[FBP0]] to %class.C**
  // CK0-DAG: [[FCP0:%.+]] = bitcast i8** [[FP0]] to %class.C**
  // CK0-DAG: store %class.C* [[VAL]], %class.C** [[FCBP0]]
  // CK0-DAG: store %class.C* [[VAL]], %class.C** [[FCP0]]
  #pragma omp target update from(mapper(id): c)
}


// CK0: define internal void [[KERNEL]](%class.C* {{.+}}[[ARG:%.+]])
// CK0: [[ADDR:%.+]] = alloca %class.C*,
// CK0: store %class.C* [[ARG]], %class.C** [[ADDR]]
// CK0: [[CADDR:%.+]] = load %class.C*, %class.C** [[ADDR]]
// CK0: [[CAADDR:%.+]] = getelementptr inbounds %class.C, %class.C* [[CADDR]], i32 0, i32 0
// CK0: [[VAL:%[^,]+]] = load i32, i32* [[CAADDR]]
// CK0: {{.+}} = add nsw i32 [[VAL]], 1
// CK0: }

#endif


///==========================================================================///
// RUN: %clang_cc1 -DCK1 -verify -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix CK1 --check-prefix CK1-64 %s
// RUN: %clang_cc1 -DCK1 -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -std=c++11 -triple powerpc64le-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -DCK1 -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix CK1 --check-prefix CK1-64 %s
// RUN: %clang_cc1 -DCK1 -verify -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix CK1 --check-prefix CK1-32 %s
// RUN: %clang_cc1 -DCK1 -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -std=c++11 -triple i386-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -DCK1 -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix CK1 --check-prefix CK1-32 %s

// RUN: %clang_cc1 -DCK1 -verify -fopenmp-simd -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix SIMD-ONLY0 %s
// RUN: %clang_cc1 -DCK1 -fopenmp-simd -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -std=c++11 -triple powerpc64le-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -DCK1 -fopenmp-simd -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix SIMD-ONLY0 %s
// RUN: %clang_cc1 -DCK1 -verify -fopenmp-simd -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix SIMD-ONLY0 %s
// RUN: %clang_cc1 -DCK1 -fopenmp-simd -fopenmp-targets=i386-pc-linux-gnu -x c++ -std=c++11 -triple i386-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -DCK1 -fopenmp-simd -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix SIMD-ONLY0 %s

#ifdef CK1

template <class T>
class C {
public:
  T a;
};

#pragma omp declare mapper(id: C<int> s) map(s.a)

// Synchronous version of mapper function.
// CK1-LABEL: define {{.*}}i32 @".omp_mapper.C<int>.id{{.*}}(i64, i8*, i8*, i{{64|32}}, i64)
// CK1-DAG: store i64 %0, i64* [[HANDLEADDR:%[^,]+]]
// CK1-DAG: store i[[sz:64|32]] %3, i{{64|32}}* [[SIZEADDR:%[^,]+]]
// CK1-DAG: store i64 %4, i64* [[TYPEADDR:%[^,]+]]
// CK1-DAG: store i8* %1, i8** [[BPTRADDR:%[^,]+]]
// CK1-DAG: store i8* %2, i8** [[VPTRADDR:%[^,]+]]
// CK1-DAG: store i32 0, i32* %retval
// CK1-DAG: [[SIZE:%.+]] = load i[[sz]], i[[sz]]* [[SIZEADDR]]
// CK1-DAG: [[TYPE:%.+]] = load i64, i64* [[TYPEADDR]]
// CK1-DAG: [[HANDLE:%.+]] = load i64, i64* [[HANDLEADDR]]
// CK1-DAG: [[PTRBEGIN:%.+]] = bitcast i8** [[VPTRADDR]] to %class.C**
// CK1-DAG: [[PTREND:%.+]] = getelementptr %class.C*, %class.C** [[PTRBEGIN]], i[[sz]] [[SIZE]]
// CK1: [[ISARRAY:%.+]] = icmp sge i[[sz]] [[SIZE]], 1
// CK1: br i1 [[ISARRAY]], label %[[INITEVALDEL:[^,]+]], label %[[LHEAD:[^,]+]]

// CK1: [[INITEVALDEL]]
// CK1: [[TYPEDEL:%.+]] = and i64 [[TYPE]], 8
// CK1: [[ISNOTDEL:%.+]] = icmp eq i64 [[TYPEDEL]], 0
// CK1: br i1 [[ISNOTDEL]], label %[[INIT:[^,]+]], label %[[LHEAD:[^,]+]]
// CK1: [[INIT]]
// CK1: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 4
// CK1-DAG: store i[[sz]] [[ARRSIZE]], i[[sz]]* [[ISIZEADDR:[^,]+]]
// CK1-DAG: [[ISIZEADDR]] = getelementptr inbounds [1 x i[[sz]]], [1 x i[[sz]]]* [[ISIZE:%.+]], i32 0, i32 0
// CK1-DAG: [[ITYPE:%.+]] = and i64 [[TYPE]], -4
// CK1-DAG: store i64 [[ITYPE]], i64* [[ITYPEADDR:[^,]+]]
// CK1-DAG: [[ITYPEADDR]] = getelementptr inbounds [1 x i64], [1 x i64]* [[ITYPE:%.+]], i32 0, i32 0
// CK1: [[IRES:%.+]] = call i32 @__tgt_target_data_mapper(i64 [[HANDLE]], i32 1, i8** [[BPTRADDR]], i8** [[VPTRADDR]], i[[sz]]* [[ISIZEADDR]], i64* [[ITYPEADDR]], i8** null)
// CK1: [[ISINITERR:%.+]] = icmp ne i32 [[IRES]], 0
// CK1: br i1 [[ISINITERR]], label %[[INITERR:[^,]+]], label %[[LHEAD:[^,]+]]
// CK1: [[INITERR]]
// CK1: store i32 [[IRES]], i32* %retval
// CK1: br label %[[DONE:[^,]+]]

// CK1: [[LHEAD]]
// CK1: [[ISEMPTY:%.+]] = icmp eq %class.C** [[PTRBEGIN]], [[PTREND]]
// CK1: br i1 [[ISEMPTY]], label %[[DONE]], label %[[LBODY:[^,]+]]
// CK1: [[LBODY]]
// CK1: [[PTR:%.+]] = phi %class.C** [ [[PTRBEGIN]], %[[LHEAD]] ], [ [[PTRNEXT:%.+]], %[[LCORRECT:[^,]+]] ]
// CK1: [[OBJ:%.+]] = load %class.C*, %class.C** [[PTR]]
// CK1-DAG: [[ABEGIN:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 0
// CK1-DAG: [[AEND:%.+]] = getelementptr i32, i32* [[ABEGIN]], i32 1
// CK1-DAG: [[ABEGINV:%.+]] = bitcast i32* [[ABEGIN]] to i8*
// CK1-DAG: [[AENDV:%.+]] = bitcast i32* [[AEND]] to i8*
// CK1-DAG: [[ABEGINI:%.+]] = ptrtoint i8* [[ABEGINV]] to i64
// CK1-DAG: [[AENDI:%.+]] = ptrtoint i8* [[AENDV]] to i64
// CK1-DAG: [[CSIZE:%.+]] = sub i64 [[AENDI]], [[ABEGINI]]
// CK1-DAG: [[CUSIZE:%.+]] = sdiv exact i64 [[CSIZE]], ptrtoint (i8* getelementptr (i8, i8* null, i32 1) to i64)
// CK1-32-DAG: [[CUSIZE32:%.+]] = trunc i64 [[CUSIZE]] to i32
// CK1-64-DAG: store i[[sz]] [[CUSIZE]], i[[sz]]* [[SIZEADDR0:%[^,]+]]
// CK1-32-DAG: store i[[sz]] [[CUSIZE32]], i[[sz]]* [[SIZEADDR0:%[^,]+]]
// CK1-DAG: [[SIZEADDR0]] = getelementptr inbounds [2 x i[[sz]]], [2 x i[[sz]]]* [[SIZEADDR:%[^,]+]], i32 0, i32 0
// CK1-DAG: store %class.C* [[OBJ]], %class.C** [[BPTRADDR0BC:%[^,]+]]
// CK1-DAG: [[BPTRADDR0BC]] = bitcast i8** [[BPTRADDR0:%.+]] to %class.C**
// CK1-DAG: [[BPTRADDR0]] = getelementptr inbounds [2 x i8*], [2 x i8*]* [[LBPTRADDR:%[^,]+]], i32 0, i32 0
// CK1-DAG: store i32* [[ABEGIN]], i32** [[PTRADDR0BC:%[^,]+]]
// CK1-DAG: [[PTRADDR0BC]] = bitcast i8** [[PTRADDR0:%.+]] to i32**
// CK1-DAG: [[PTRADDR0]] = getelementptr inbounds [2 x i8*], [2 x i8*]* [[PTRADDR:%[^,]+]], i32 0, i32 0
// CK1-DAG: [[TYPETF:%.+]] = and i64 [[TYPE]], 3
// CK1-DAG: [[ISALLOC:%.+]] = icmp eq i64 [[TYPETF]], 0
// CK1-DAG: br i1 [[ISALLOC]], label %[[ALLOC:[^,]+]], label %[[ALLOCELSE:[^,]+]]
// CK1-DAG: [[ALLOC]]
// CK1-DAG: br label %[[TYEND:[^,]+]]
// CK1-DAG: [[ALLOCELSE]]
// CK1-DAG: [[ISTO:%.+]] = icmp eq i64 [[TYPETF]], 1
// CK1-DAG: br i1 [[ISTO]], label %[[TO:[^,]+]], label %[[TOELSE:[^,]+]]
// CK1-DAG: [[TO]]
// CK1-DAG: br label %[[TYEND]]
// CK1-DAG: [[TOELSE]]
// CK1-DAG: [[ISFROM:%.+]] = icmp eq i64 [[TYPETF]], 2
// CK1-DAG: br i1 [[ISFROM]], label %[[FROM:[^,]+]], label %[[TYEND]]
// CK1-DAG: [[FROM]]
// CK1-DAG: br label %[[TYEND]]
// CK1-DAG: [[TYEND]]
// CK1-DAG: [[PHITYPE0:%.+]] = phi i64 [ 32, %[[ALLOC]] ], [ 32, %[[TO]] ], [ 32, %[[FROM]] ], [ 32, %[[TOELSE]] ]
// CK1-DAG: store i64 [[PHITYPE0]], i64* [[TYPEADDR0:%[^,]+]]
// CK1-DAG: [[TYPEADDR0]] = getelementptr inbounds [2 x i64], [2 x i64]* [[TYPEADDR:%[^,]+]], i32 0, i32 0
// CK1-DAG: [[BPTRADDR1:%.+]] = getelementptr inbounds [2 x i8*], [2 x i8*]* [[LBPTRADDR]], i32 0, i32 1
// CK1-DAG: [[BPTRADDR1BC:%.+]] = bitcast i8** [[BPTRADDR1]] to %class.C**
// CK1-DAG: store %class.C* [[OBJ]], %class.C** [[BPTRADDR1BC]]
// CK1-DAG: [[PTRADDR1:%.+]] = getelementptr inbounds [2 x i8*], [2 x i8*]* [[PTRADDR]], i32 0, i32 1
// CK1-DAG: [[PTRADDR1BC:%.+]] = bitcast i8** [[PTRADDR1]] to i32**
// CK1-DAG: store i32* [[ABEGIN]], i32** [[PTRADDR1BC]]
// CK1-DAG: [[SIZEADDR1:%.+]] = getelementptr inbounds [2 x i[[sz]]], [2 x i[[sz]]]* [[SIZEADDR]], i32 0, i32 1
// CK1-DAG: store i[[sz]] 4, i[[sz]]* [[SIZEADDR1]]
// CK1-DAG: [[TYPEADDR1:%.+]] = getelementptr inbounds [2 x i64], [2 x i64]* [[TYPEADDR]], i32 0, i32 1
// CK1-DAG: [[TYPETF:%.+]] = and i64 [[TYPE]], 3
// CK1-DAG: [[ISALLOC:%.+]] = icmp eq i64 [[TYPETF]], 0
// CK1-DAG: br i1 [[ISALLOC]], label %[[ALLOC:[^,]+]], label %[[ALLOCELSE:[^,]+]]
// CK1-DAG: [[ALLOC]]
// CK1-DAG: br label %[[TYEND:[^,]+]]
// CK1-DAG: [[ALLOCELSE]]
// CK1-DAG: [[ISTO:%.+]] = icmp eq i64 [[TYPETF]], 1
// CK1-DAG: br i1 [[ISTO]], label %[[TO:[^,]+]], label %[[TOELSE:[^,]+]]
// CK1-DAG: [[TO]]
// CK1-DAG: br label %[[TYEND]]
// CK1-DAG: [[TOELSE]]
// CK1-DAG: [[ISFROM:%.+]] = icmp eq i64 [[TYPETF]], 2
// CK1-DAG: br i1 [[ISFROM]], label %[[FROM:[^,]+]], label %[[TYEND]]
// CK1-DAG: [[FROM]]
// CK1-DAG: br label %[[TYEND]]
// CK1-DAG: [[TYEND]]
// 0x1,000,000,000; 0x1,000,000,001; 0x1,000,000,002; 0x1,000,000,003
// CK1-DAG: [[TYPE1:%.+]] = phi i64 [ 281474976710656, %[[ALLOC]] ], [ 281474976710657, %[[TO]] ], [ 281474976710658, %[[FROM]] ], [ 281474976710659, %[[TOELSE]] ]
// CK1-DAG: store i64 [[TYPE1]], i64* [[TYPEADDR1]]
// CK1-DAG: [[ARGBPTR:%.+]] = getelementptr inbounds [2 x i8*], [2 x i8*]* [[LBPTRADDR]], i32 0, i32 0
// CK1-DAG: [[ARGPTR:%.+]] = getelementptr inbounds [2 x i8*], [2 x i8*]* [[PTRADDR]], i32 0, i32 0
// CK1-DAG: [[ARGSIZE:%.+]] = getelementptr inbounds [2 x i[[sz]]], [2 x i[[sz]]]* [[SIZEADDR]], i32 0, i32 0
// CK1-DAG: [[ARGTYPE:%.+]] = getelementptr inbounds [2 x i64], [2 x i64]* [[TYPEADDR]], i32 0, i32 0
// CK1: [[RES:%.+]] = call i32 @__tgt_target_data_mapper(i64 [[HANDLE]], i32 2, i8** [[ARGBPTR]], i8** [[ARGPTR]], i[[sz]]* [[ARGSIZE]], i64* [[ARGTYPE]], i8** null)
// CK1: [[ISERR:%.+]] = icmp ne i32 [[RES]], 0
// CK1: br i1 [[ISERR]], label %[[LERR:[^,]+]], label %[[LCORRECT]]
// CK1: [[LERR]]
// CK1: store i32 [[RES]], i32* %retval
// CK1: br label %[[DONE]]
// CK1: [[LCORRECT]]
// CK1: [[PTRNEXT]] = getelementptr %class.C*, %class.C** [[PTR]], i32 1
// CK1: [[ISDONE:%.+]] = icmp eq %class.C** [[PTRNEXT]], [[PTREND]]
// CK1: br i1 [[ISDONE]], label %[[LEXIT:[^,]+]], label %[[LBODY]]

// CK1: [[LEXIT]]
// CK1: br i1 [[ISARRAY]], label %[[EVALDEL:[^,]+]], label %[[DONE]]
// CK1: [[EVALDEL]]
// CK1: [[TYPEDEL:%.+]] = and i64 [[TYPE]], 8
// CK1: [[ISDEL:%.+]] = icmp ne i64 [[TYPEDEL]], 0
// CK1: br i1 [[ISDEL]], label %[[DEL:[^,]+]], label %[[DONE]]
// CK1: [[DEL]]
// CK1: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 4
// CK1-DAG: store i[[sz]] [[ARRSIZE]], i[[sz]]* [[DSIZEADDR:%[^,]+]]
// CK1-DAG: [[DSIZEADDR]] = getelementptr inbounds [1 x i[[sz]]], [1 x i[[sz]]]* [[DSIZE:%.+]], i32 0, i32 0
// CK1-DAG: [[DTYPE:%.+]] = and i64 [[TYPE]], -4
// CK1-DAG: store i64 [[DTYPE]], i64* [[DTYPEADDR:%[^,]+]]
// CK1-DAG: [[DTYPEADDR]] = getelementptr inbounds [1 x i64], [1 x i64]* [[DTYPE:%.+]], i32 0, i32 0
// CK1: [[DRES:%.+]] = call i32 @__tgt_target_data_mapper(i64 [[HANDLE]], i32 1, i8** [[BPTRADDR]], i8** [[VPTRADDR]], i[[sz]]* [[DSIZEADDR]], i64* [[DTYPEADDR]], i8** null)
// CK1: [[ISDELERR:%.+]] = icmp ne i32 [[DRES]], 0
// CK1: br i1 [[ISDELERR]], label %[[DELERR:[^,]+]], label %[[DONE]]
// CK1: [[DELERR]]
// CK1: store i32 [[DRES]], i32* %retval
// CK1: br label %[[DONE]]
// CK1: [[DONE]]
// CK1: [[RET:%.+]] = load i32, i32* %retval
// CK1: ret i32 [[RET]]

#endif

#endif
