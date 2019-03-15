///==========================================================================///
// RUN: %clang_cc1 -verify -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck %s
// RUN: %clang_cc1 -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -std=c++11 -triple powerpc64le-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck %s
// RUN: %clang_cc1 -verify -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck %s
// RUN: %clang_cc1 -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -std=c++11 -triple i386-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck %s

// RUN: %clang_cc1 -verify -fopenmp-simd -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix SIMD-ONLY0 %s
// RUN: %clang_cc1 -fopenmp-simd -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -std=c++11 -triple powerpc64le-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -fopenmp-simd -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix SIMD-ONLY0 %s
// RUN: %clang_cc1 -verify -fopenmp-simd -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix SIMD-ONLY0 %s
// RUN: %clang_cc1 -fopenmp-simd -fopenmp-targets=i386-pc-linux-gnu -x c++ -std=c++11 -triple i386-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -fopenmp-simd -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix SIMD-ONLY0 %s

// SIMD-ONLY0-NOT: {{__kmpc|__tgt}}

// expected-no-diagnostics
#ifndef HEADER
#define HEADER

// CHECK-LABEL: @.__omp_offloading_{{.*}}foo{{.*}}.region_id = weak constant i8 0

// CHECK: [[SIZES:@.+]] = {{.+}}constant [1 x i[[sz:64|32]]] [i{{64|32}} 4]
// CHECK: [[TYPES:@.+]] = {{.+}}constant [1 x i64] [i64 35]
// CHECK: [[TSIZES:@.+]] = {{.+}}constant [1 x i[[sz]]] [i[[sz]] 4]
// CHECK: [[TTYPES:@.+]] = {{.+}}constant [1 x i64] [i64 33]
// CHECK: [[FSIZES:@.+]] = {{.+}}constant [1 x i[[sz]]] [i[[sz]] 4]
// CHECK: [[FTYPES:@.+]] = {{.+}}constant [1 x i64] [i64 34]

class C {
public:
  int a;
};

#pragma omp declare mapper(id: C s) map(s.a)

// CHECK-LABEL: define {{.*}}i32 @.omp_mapper.class_C.id{{.*}}(i64, i8*, i8*, i{{64|32}}, i64)
// CHECK-DAG: store i64 %0, i64* [[DIDADDR:%[^,]+]]
// CHECK-DAG: store i[[sz]] %3, i[[sz]]* [[SIZEADDR:%[^,]+]]
// CHECK-DAG: store i64 %4, i64* [[TYPEADDR:%[^,]+]]
// CHECK-DAG: store i8* %1, i8** [[BPTRADDR:%[^,]+]]
// CHECK-DAG: store i8* %2, i8** [[VPTRADDR:%[^,]+]]
// CHECK-DAG: store i32 0, i32* %retval
// CHECK-DAG: [[SIZE:%.+]] = load i[[sz]], i[[sz]]* [[SIZEADDR]]
// CHECK-DAG: [[TYPE:%.+]] = load i64, i64* [[TYPEADDR]]
// CHECK-DAG: [[DID:%.+]] = load i64, i64* [[DIDADDR]]
// CHECK-DAG: [[PTRBEGIN:%.+]] = bitcast i8** [[VPTRADDR]] to %class.C**
// CHECK-DAG: [[PTREND:%.+]] = getelementptr %class.C*, %class.C** [[PTRBEGIN]], i[[sz]] [[SIZE]]
// CHECK: [[ISARRAY:%.+]] = icmp sge i[[sz]] [[SIZE]], 1
// CHECK: br i1 [[ISARRAY]], label %[[INITEVALDEL:[^,]+]], label %[[LHEAD:[^,]+]]

// CHECK: [[INITEVALDEL]]
// CHECK: [[TYPEDEL:%.+]] = and i64 [[TYPE]], 8
// CHECK: [[ISNOTDEL:%.+]] = icmp eq i64 [[TYPEDEL]], 0
// CHECK: br i1 [[ISNOTDEL]], label %[[INIT:[^,]+]], label %[[LHEAD:[^,]+]]
// CHECK: [[INIT]]
// CHECK: [[ASIZE:%.+]] = mul i[[sz]] [[SIZE]], 4
// CHECK: [[ISIZEADDR:%.+]] = getelementptr inbounds [1 x i[[sz]]], [1 x i[[sz]]]* [[ISIZE:%.+]], i32 0, i32 0
// CHECK: store i[[sz]] [[ASIZE]], i[[sz]]* [[ISIZEADDR]]
// CHECK: [[ITYPEADDR:%.+]] = getelementptr inbounds [1 x i64], [1 x i64]* [[ITYPE:%.+]], i32 0, i32 0
// CHECK: [[ITYPE:%.+]] = and i64 [[TYPE]], -4
// CHECK: store i64 [[ITYPE]], i64* [[ITYPEADDR]]
// CHECK: [[IRES:%.+]] = call i32 @__tgt_target_data_mapper(i64 [[DID]], i32 1, i8** [[BPTRADDR]], i8** [[VPTRADDR]], i[[sz]]* [[ISIZEADDR]], i64* [[ITYPEADDR]], i8** null)
// CHECK: [[ISINITERR:%.+]] = icmp ne i32 [[IRES]], 0
// CHECK: br i1 [[ISINITERR]], label %[[INITERR:[^,]+]], label %[[LHEAD:[^,]+]]
// CHECK: [[INITERR]]
// CHECK: store i32 [[IRES]], i32* %retval
// CHECK: br label %[[DONE:[^,]+]]

// CHECK: [[LHEAD]]
// CHECK: [[ISEMPTY:%.+]] = icmp eq %class.C** [[PTRBEGIN]], [[PTREND]]
// CHECK: br i1 [[ISEMPTY]], label %[[DONE]], label %[[LBODY:[^,]+]]
// CHECK: [[LBODY]]
// CHECK: [[PTR:%.+]] = phi %class.C** [ [[PTRBEGIN]], %[[LHEAD]] ], [ [[PTRNEXT:%.+]], %[[LCORRECT:[^,]+]] ]
// CHECK: [[OBJ:%.+]] = load %class.C*, %class.C** [[PTR]]
// CHECK: [[APTR:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 0
// CHECK: [[LCORRECT]]

// CHECK: [[DONE]]

// CHECK-LABEL: define {{.*}}i32 @.omp_mapper.class_C.id{{.*}}nowait{{.*}}(i64, i8*, i8*, i{{64|32}}, i64)

// CHECK-LABEL: define {{.*}}void @{{.*}}foo{{.*}}
void foo(int a){
  int i = a;
  C c;
  c.a = a;

  // CHECK-DAG: call i32 @__tgt_target(i64 {{.+}}, i8* {{.+}}, i32 1, i8** [[BPGEP:%[0-9]+]], i8** [[PGEP:%[0-9]+]], {{.+}}[[SIZES]]{{.+}}, {{.+}}[[TYPES]]{{.+}})
  // CHECK-DAG: [[BPGEP]] = getelementptr inbounds {{.+}}[[BPS:%[^,]+]], i32 0, i32 0
  // CHECK-DAG: [[PGEP]] = getelementptr inbounds {{.+}}[[PS:%[^,]+]], i32 0, i32 0
  // CHECK-DAG: [[BP1:%.+]] = getelementptr inbounds {{.+}}[[BPS]], i32 0, i32 0
  // CHECK-DAG: [[P1:%.+]] = getelementptr inbounds {{.+}}[[PS]], i32 0, i32 0
  // CHECK-DAG: [[CBP1:%.+]] = bitcast i8** [[BP1]] to %class.C**
  // CHECK-DAG: [[CP1:%.+]] = bitcast i8** [[P1]] to %class.C**
  // CHECK-DAG: store %class.C* [[VAL:%[^,]+]], %class.C** [[CBP1]]
  // CHECK-DAG: store %class.C* [[VAL]], %class.C** [[CP1]]
  // CHECK: call void [[KERNEL:@.+]](%class.C* [[VAL]])
  #pragma omp target map(mapper(id),tofrom: c)
  {
   ++c.a;
  }

  // CHECK-DAG: call void @__tgt_target_data_update(i64 -1, i32 1, i8** [[TGEPBP:%.+]], i8** [[TGEPP:%.+]], i[[sz]]* getelementptr {{.+}}[1 x i[[sz]]]* [[TSIZES]], i32 0, i32 0), {{.+}}getelementptr {{.+}}[1 x i64]* [[TTYPES]]{{.+}})
  // CHECK-DAG: [[TGEPBP]] = getelementptr inbounds {{.+}}[[TBP:%[^,]+]], i{{.+}} 0, i{{.+}} 0
  // CHECK-DAG: [[TGEPP]] = getelementptr inbounds {{.+}}[[TP:%[^,]+]], i{{.+}} 0, i{{.+}} 0
  // CHECK-DAG: [[TBP0:%.+]] = getelementptr inbounds {{.+}}[[TBP]], i{{.+}} 0, i{{.+}} 0
  // CHECK-DAG: [[TP0:%.+]] = getelementptr inbounds {{.+}}[[TP]], i{{.+}} 0, i{{.+}} 0
  // CHECK-DAG: [[TCBP0:%.+]] = bitcast i8** [[TBP0]] to %class.C**
  // CHECK-DAG: [[TCP0:%.+]] = bitcast i8** [[TP0]] to %class.C**
  // CHECK-DAG: store %class.C* [[VAL]], %class.C** [[TCBP0]]
  // CHECK-DAG: store %class.C* [[VAL]], %class.C** [[TCP0]]
  #pragma omp target update to(mapper(id): c)

  // CHECK-DAG: call void @__tgt_target_data_update(i64 -1, i32 1, i8** [[FGEPBP:%.+]], i8** [[FGEPP:%.+]], i[[sz]]* getelementptr {{.+}}[1 x i[[sz]]]* [[FSIZES]], i32 0, i32 0), {{.+}}getelementptr {{.+}}[1 x i64]* [[FTYPES]]{{.+}})
  // CHECK-DAG: [[FGEPBP]] = getelementptr inbounds {{.+}}[[FBP:%[^,]+]], i{{.+}} 0, i{{.+}} 0
  // CHECK-DAG: [[FGEPP]] = getelementptr inbounds {{.+}}[[FP:%[^,]+]], i{{.+}} 0, i{{.+}} 0
  // CHECK-DAG: [[FBP0:%.+]] = getelementptr inbounds {{.+}}[[FBP]], i{{.+}} 0, i{{.+}} 0
  // CHECK-DAG: [[FP0:%.+]] = getelementptr inbounds {{.+}}[[FP]], i{{.+}} 0, i{{.+}} 0
  // CHECK-DAG: [[FCBP0:%.+]] = bitcast i8** [[FBP0]] to %class.C**
  // CHECK-DAG: [[FCP0:%.+]] = bitcast i8** [[FP0]] to %class.C**
  // CHECK-DAG: store %class.C* [[VAL]], %class.C** [[FCBP0]]
  // CHECK-DAG: store %class.C* [[VAL]], %class.C** [[FCP0]]
  #pragma omp target update from(mapper(id): c)
}


// CHECK: define internal void [[KERNEL]](%class.C* {{.+}}[[ARG:%.+]])
// CHECK: [[ADDR:%.+]] = alloca %class.C*,
// CHECK: store %class.C* [[ARG]], %class.C** [[ADDR]]
// CHECK: [[CADDR:%.+]] = load %class.C*, %class.C** [[ADDR]]
// CHECK: [[CAADDR:%.+]] = getelementptr inbounds %class.C, %class.C* [[CADDR]], i32 0, i32 0
// CHECK: [[VAL:%[^,]+]] = load i32, i32* [[CAADDR]]
// CHECK: {{.+}} = add nsw i32 [[VAL]], 1
// CHECK: }

#endif
