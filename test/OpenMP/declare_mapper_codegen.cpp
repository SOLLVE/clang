///==========================================================================///
// RUN: %clang_cc1 -verify -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix CHECK --check-prefix CHECK-64 %s
// RUN: %clang_cc1 -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -std=c++11 -triple powerpc64le-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -fopenmp -fopenmp-targets=powerpc64le-ibm-linux-gnu -x c++ -triple powerpc64le-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix CHECK --check-prefix CHECK-64 %s
// RUN: %clang_cc1 -verify -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -emit-llvm -femit-all-decls -disable-llvm-passes %s -o - | FileCheck --check-prefix CHECK --check-prefix CHECK-32 %s
// RUN: %clang_cc1 -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -std=c++11 -triple i386-unknown-unknown -emit-pch -femit-all-decls -disable-llvm-passes -o %t %s
// RUN: %clang_cc1 -fopenmp -fopenmp-targets=i386-pc-linux-gnu -x c++ -triple i386-unknown-unknown -std=c++11 -femit-all-decls -disable-llvm-passes -include-pch %t -verify %s -emit-llvm -o - | FileCheck --check-prefix CHECK --check-prefix CHECK-32 %s

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
// CHECK-64: [[SIZES:@.+]] = {{.+}}constant [1 x i[[sz:64|32]]] [i64 16]
// CHECK-32: [[SIZES:@.+]] = {{.+}}constant [1 x i[[sz:64|32]]] [i32 8]
// CHECK: [[TYPES:@.+]] = {{.+}}constant [1 x i64] [i64 35]
// CHECK-64: [[TSIZES:@.+]] = {{.+}}constant [1 x i[[sz]]] [i[[sz]] 16]
// CHECK-32: [[TSIZES:@.+]] = {{.+}}constant [1 x i[[sz]]] [i[[sz]] 8]
// CHECK: [[TTYPES:@.+]] = {{.+}}constant [1 x i64] [i64 33]
// CHECK-64: [[FSIZES:@.+]] = {{.+}}constant [1 x i[[sz]]] [i[[sz]] 16]
// CHECK-32: [[FSIZES:@.+]] = {{.+}}constant [1 x i[[sz]]] [i[[sz]] 8]
// CHECK: [[FTYPES:@.+]] = {{.+}}constant [1 x i64] [i64 34]

class C {
public:
  int a;
  double *b;
};

#pragma omp declare mapper(id: C s) map(s.a, s.b[0:2])

// Synchronous version of mapper function.
// CHECK-LABEL: define {{.*}}i32 @.omp_mapper.class_C.id{{.*}}(i64, i8*, i8*, i{{64|32}}, i64)
// CHECK-DAG: store i64 %0, i64* [[DIDADDR:%[^,]+]]
// CHECK-DAG: store i[[sz:64|32]] %3, i{{64|32}}* [[SIZEADDR:%[^,]+]]
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
// CHECK-64: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 16
// CHECK-32: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 8
// CHECK-DAG: store i[[sz]] [[ARRSIZE]], i[[sz]]* [[ISIZEADDR:[^,]+]]
// CHECK-DAG: [[ISIZEADDR]] = getelementptr inbounds [1 x i[[sz]]], [1 x i[[sz]]]* [[ISIZE:%.+]], i32 0, i32 0
// CHECK-DAG: [[ITYPE:%.+]] = and i64 [[TYPE]], -4
// CHECK-DAG: store i64 [[ITYPE]], i64* [[ITYPEADDR:[^,]+]]
// CHECK-DAG: [[ITYPEADDR]] = getelementptr inbounds [1 x i64], [1 x i64]* [[ITYPE:%.+]], i32 0, i32 0
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
// CHECK-DAG: [[ABEGIN:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 0
// CHECK-DAG: [[BBEGIN:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 1
// CHECK-DAG: [[BBEGIN2:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 1
// CHECK-DAG: [[BARRBEGIN:%.+]] = load double*, double** [[BBEGIN2]]
// CHECK-DAG: [[BARRBEGINGEP:%.+]] = getelementptr inbounds double, double* [[BARRBEGIN]], i[[sz]] 0
// CHECK-DAG: [[BEND:%.+]] = getelementptr double*, double** [[BBEGIN]], i32 1
// CHECK-DAG: [[ABEGINV:%.+]] = bitcast i32* [[ABEGIN]] to i8*
// CHECK-DAG: [[BENDV:%.+]] = bitcast double** [[BEND]] to i8*
// CHECK-DAG: [[ABEGINI:%.+]] = ptrtoint i8* [[ABEGINV]] to i64
// CHECK-DAG: [[BENDI:%.+]] = ptrtoint i8* [[BENDV]] to i64
// CHECK-DAG: [[CSIZE:%.+]] = sub i64 [[BENDI]], [[ABEGINI]]
// CHECK-DAG: [[CUSIZE:%.+]] = sdiv exact i64 [[CSIZE]], ptrtoint (i8* getelementptr (i8, i8* null, i32 1) to i64)
// CHECK-32-DAG: [[CUSIZE32:%.+]] = trunc i64 [[CUSIZE]] to i32
// CHECK-64-DAG: store i[[sz]] [[CUSIZE]], i[[sz]]* [[SIZEADDR0:%[^,]+]]
// CHECK-32-DAG: store i[[sz]] [[CUSIZE32]], i[[sz]]* [[SIZEADDR0:%[^,]+]]
// CHECK-DAG: [[SIZEADDR0]] = getelementptr inbounds [3 x i[[sz]]], [3 x i[[sz]]]* [[SIZEADDR:%[^,]+]], i32 0, i32 0
// CHECK-DAG: store %class.C* [[OBJ]], %class.C** [[BPTRADDR0BC:%[^,]+]]
// CHECK-DAG: [[BPTRADDR0BC]] = bitcast i8** [[BPTRADDR0:%.+]] to %class.C**
// CHECK-DAG: [[BPTRADDR0]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[LBPTRADDR:%[^,]+]], i32 0, i32 0
// CHECK-DAG: store i32* [[ABEGIN]], i32** [[PTRADDR0BC:%[^,]+]]
// CHECK-DAG: [[PTRADDR0BC]] = bitcast i8** [[PTRADDR0:%.+]] to i32**
// CHECK-DAG: [[PTRADDR0]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[PTRADDR:%[^,]+]], i32 0, i32 0
// CHECK-DAG: [[TYPETF:%.+]] = and i64 [[TYPE]], 3
// CHECK-DAG: [[ISALLOC:%.+]] = icmp eq i64 [[TYPETF]], 0
// CHECK-DAG: br i1 [[ISALLOC]], label %[[ALLOC:[^,]+]], label %[[ALLOCELSE:[^,]+]]
// CHECK-DAG: [[ALLOC]]
// CHECK-DAG: br label %[[TYEND:[^,]+]]
// CHECK-DAG: [[ALLOCELSE]]
// CHECK-DAG: [[ISTO:%.+]] = icmp eq i64 [[TYPETF]], 1
// CHECK-DAG: br i1 [[ISTO]], label %[[TO:[^,]+]], label %[[TOELSE:[^,]+]]
// CHECK-DAG: [[TO]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TOELSE]]
// CHECK-DAG: [[ISFROM:%.+]] = icmp eq i64 [[TYPETF]], 2
// CHECK-DAG: br i1 [[ISFROM]], label %[[FROM:[^,]+]], label %[[TYEND]]
// CHECK-DAG: [[FROM]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TYEND]]
// CHECK-DAG: [[PHITYPE0:%.+]] = phi i64 [ 32, %[[ALLOC]] ], [ 32, %[[TO]] ], [ 32, %[[FROM]] ], [ 32, %[[TOELSE]] ]
// CHECK-DAG: store i64 [[PHITYPE0]], i64* [[TYPEADDR0:%[^,]+]]
// CHECK-DAG: [[TYPEADDR0]] = getelementptr inbounds [3 x i64], [3 x i64]* [[TYPEADDR:%[^,]+]], i32 0, i32 0
// CHECK-DAG: [[BPTRADDR1:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[LBPTRADDR]], i32 0, i32 1
// CHECK-DAG: [[BPTRADDR1BC:%.+]] = bitcast i8** [[BPTRADDR1]] to %class.C**
// CHECK-DAG: store %class.C* [[OBJ]], %class.C** [[BPTRADDR1BC]]
// CHECK-DAG: [[PTRADDR1:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[PTRADDR]], i32 0, i32 1
// CHECK-DAG: [[PTRADDR1BC:%.+]] = bitcast i8** [[PTRADDR1]] to i32**
// CHECK-DAG: store i32* [[ABEGIN]], i32** [[PTRADDR1BC]]
// CHECK-DAG: [[SIZEADDR1:%.+]] = getelementptr inbounds [3 x i[[sz]]], [3 x i[[sz]]]* [[SIZEADDR]], i32 0, i32 1
// CHECK-DAG: store i[[sz]] 4, i[[sz]]* [[SIZEADDR1]]
// CHECK-DAG: [[TYPEADDR1:%.+]] = getelementptr inbounds [3 x i64], [3 x i64]* [[TYPEADDR]], i32 0, i32 1
// CHECK-DAG: [[TYPETF:%.+]] = and i64 [[TYPE]], 3
// CHECK-DAG: [[ISALLOC:%.+]] = icmp eq i64 [[TYPETF]], 0
// CHECK-DAG: br i1 [[ISALLOC]], label %[[ALLOC:[^,]+]], label %[[ALLOCELSE:[^,]+]]
// CHECK-DAG: [[ALLOC]]
// CHECK-DAG: br label %[[TYEND:[^,]+]]
// CHECK-DAG: [[ALLOCELSE]]
// CHECK-DAG: [[ISTO:%.+]] = icmp eq i64 [[TYPETF]], 1
// CHECK-DAG: br i1 [[ISTO]], label %[[TO:[^,]+]], label %[[TOELSE:[^,]+]]
// CHECK-DAG: [[TO]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TOELSE]]
// CHECK-DAG: [[ISFROM:%.+]] = icmp eq i64 [[TYPETF]], 2
// CHECK-DAG: br i1 [[ISFROM]], label %[[FROM:[^,]+]], label %[[TYEND]]
// CHECK-DAG: [[FROM]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TYEND]]
// 0x1,000,000,000; 0x1,000,000,001; 0x1,000,000,002; 0x1,000,000,003
// CHECK-DAG: [[TYPE1:%.+]] = phi i64 [ 281474976710656, %[[ALLOC]] ], [ 281474976710657, %[[TO]] ], [ 281474976710658, %[[FROM]] ], [ 281474976710659, %[[TOELSE]] ]
// CHECK-DAG: store i64 [[TYPE1]], i64* [[TYPEADDR1]]
// CHECK-DAG: [[BPTRADDR2:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[LBPTRADDR]], i32 0, i32 2
// CHECK-DAG: [[BPTRADDR2BC:%.+]] = bitcast i8** [[BPTRADDR2]] to double***
// CHECK-DAG: store double** [[BBEGIN]], double*** [[BPTRADDR2BC]]
// CHECK-DAG: [[PTRADDR2:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[PTRADDR]], i32 0, i32 2
// CHECK-DAG: [[PTRADDR2BC:%.+]] = bitcast i8** [[PTRADDR2]] to double**
// CHECK-DAG: store double* [[BARRBEGINGEP]], double** [[PTRADDR2BC]]
// CHECK-DAG: [[SIZEADDR2:%.+]] = getelementptr inbounds [3 x i[[sz]]], [3 x i[[sz]]]* [[SIZEADDR]], i32 0, i32 2
// CHECK-DAG: store i[[sz]] 16, i[[sz]]* [[SIZEADDR2]]
// CHECK-DAG: [[TYPEADDR2:%.+]] = getelementptr inbounds [3 x i64], [3 x i64]* [[TYPEADDR]], i32 0, i32 2
// CHECK-DAG: [[TYPETF:%.+]] = and i64 [[TYPE]], 3
// CHECK-DAG: [[ISALLOC:%.+]] = icmp eq i64 [[TYPETF]], 0
// CHECK-DAG: br i1 [[ISALLOC]], label %[[ALLOC:[^,]+]], label %[[ALLOCELSE:[^,]+]]
// CHECK-DAG: [[ALLOC]]
// CHECK-DAG: br label %[[TYEND:[^,]+]]
// CHECK-DAG: [[ALLOCELSE]]
// CHECK-DAG: [[ISTO:%.+]] = icmp eq i64 [[TYPETF]], 1
// CHECK-DAG: br i1 [[ISTO]], label %[[TO:[^,]+]], label %[[TOELSE:[^,]+]]
// CHECK-DAG: [[TO]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TOELSE]]
// CHECK-DAG: [[ISFROM:%.+]] = icmp eq i64 [[TYPETF]], 2
// CHECK-DAG: br i1 [[ISFROM]], label %[[FROM:[^,]+]], label %[[TYEND]]
// CHECK-DAG: [[FROM]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TYEND]]
// 0x1,000,000,010; 0x1,000,000,011; 0x1,000,000,012; 0x1,000,000,013
// CHECK-DAG: [[TYPE2:%.+]] = phi i64 [ 281474976710672, %[[ALLOC]] ], [ 281474976710673, %[[TO]] ], [ 281474976710674, %[[FROM]] ], [ 281474976710675, %[[TOELSE]] ]
// CHECK-DAG: store i64 [[TYPE2]], i64* [[TYPEADDR2]]
// CHECK-DAG: [[ARGBPTR:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[LBPTRADDR]], i32 0, i32 0
// CHECK-DAG: [[ARGPTR:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[PTRADDR]], i32 0, i32 0
// CHECK-DAG: [[ARGSIZE:%.+]] = getelementptr inbounds [3 x i[[sz]]], [3 x i[[sz]]]* [[SIZEADDR]], i32 0, i32 0
// CHECK-DAG: [[ARGTYPE:%.+]] = getelementptr inbounds [3 x i64], [3 x i64]* [[TYPEADDR]], i32 0, i32 0
// CHECK: [[RES:%.+]] = call i32 @__tgt_target_data_mapper(i64 [[DID]], i32 3, i8** [[ARGBPTR]], i8** [[ARGPTR]], i[[sz]]* [[ARGSIZE]], i64* [[ARGTYPE]], i8** null)
// CHECK: [[ISERR:%.+]] = icmp ne i32 [[RES]], 0
// CHECK: br i1 [[ISERR]], label %[[LERR:[^,]+]], label %[[LCORRECT]]
// CHECK: [[LERR]]
// CHECK: store i32 [[RES]], i32* %retval
// CHECK: br label %[[DONE]]
// CHECK: [[LCORRECT]]
// CHECK: [[PTRNEXT]] = getelementptr %class.C*, %class.C** [[PTR]], i32 1
// CHECK: [[ISDONE:%.+]] = icmp eq %class.C** [[PTRNEXT]], [[PTREND]]
// CHECK: br i1 [[ISDONE]], label %[[LEXIT:[^,]+]], label %[[LBODY]]

// CHECK: [[LEXIT]]
// CHECK: br i1 [[ISARRAY]], label %[[EVALDEL:[^,]+]], label %[[DONE]]
// CHECK: [[EVALDEL]]
// CHECK: [[TYPEDEL:%.+]] = and i64 [[TYPE]], 8
// CHECK: [[ISDEL:%.+]] = icmp ne i64 [[TYPEDEL]], 0
// CHECK: br i1 [[ISDEL]], label %[[DEL:[^,]+]], label %[[DONE]]
// CHECK: [[DEL]]
// CHECK-64: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 16
// CHECK-32: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 8
// CHECK-DAG: store i[[sz]] [[ARRSIZE]], i[[sz]]* [[DSIZEADDR:%[^,]+]]
// CHECK-DAG: [[DSIZEADDR]] = getelementptr inbounds [1 x i[[sz]]], [1 x i[[sz]]]* [[DSIZE:%.+]], i32 0, i32 0
// CHECK-DAG: [[DTYPE:%.+]] = and i64 [[TYPE]], -4
// CHECK-DAG: store i64 [[DTYPE]], i64* [[DTYPEADDR:%[^,]+]]
// CHECK-DAG: [[DTYPEADDR]] = getelementptr inbounds [1 x i64], [1 x i64]* [[DTYPE:%.+]], i32 0, i32 0
// CHECK: [[DRES:%.+]] = call i32 @__tgt_target_data_mapper(i64 [[DID]], i32 1, i8** [[BPTRADDR]], i8** [[VPTRADDR]], i[[sz]]* [[DSIZEADDR]], i64* [[DTYPEADDR]], i8** null)
// CHECK: [[ISDELERR:%.+]] = icmp ne i32 [[DRES]], 0
// CHECK: br i1 [[ISDELERR]], label %[[DELERR:[^,]+]], label %[[DONE]]
// CHECK: [[DELERR]]
// CHECK: store i32 [[DRES]], i32* %retval
// CHECK: br label %[[DONE]]
// CHECK: [[DONE]]
// CHECK: [[RET:%.+]] = load i32, i32* %retval
// CHECK: ret i32 [[RET]]


// Asynchronous version of mapper function.
// CHECK-LABEL: define {{.*}}i32 @.omp_mapper.class_C.id{{.*}}nowait{{.*}}(i64, i8*, i8*, i{{64|32}}, i64)
// CHECK-DAG: store i64 %0, i64* [[DIDADDR:%[^,]+]]
// CHECK-DAG: store i[[sz:64|32]] %3, i{{64|32}}* [[SIZEADDR:%[^,]+]]
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
// CHECK-64: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 16
// CHECK-32: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 8
// CHECK-DAG: store i[[sz]] [[ARRSIZE]], i[[sz]]* [[ISIZEADDR:[^,]+]]
// CHECK-DAG: [[ISIZEADDR]] = getelementptr inbounds [1 x i[[sz]]], [1 x i[[sz]]]* [[ISIZE:%.+]], i32 0, i32 0
// CHECK-DAG: [[ITYPE:%.+]] = and i64 [[TYPE]], -4
// CHECK-DAG: store i64 [[ITYPE]], i64* [[ITYPEADDR:[^,]+]]
// CHECK-DAG: [[ITYPEADDR]] = getelementptr inbounds [1 x i64], [1 x i64]* [[ITYPE:%.+]], i32 0, i32 0
// CHECK: [[IRES:%.+]] = call i32 @__tgt_target_data_mapper_nowait(i64 [[DID]], i32 1, i8** [[BPTRADDR]], i8** [[VPTRADDR]], i[[sz]]* [[ISIZEADDR]], i64* [[ITYPEADDR]], i8** null)
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
// CHECK-DAG: [[ABEGIN:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 0
// CHECK-DAG: [[BBEGIN:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 1
// CHECK-DAG: [[BBEGIN2:%.+]] = getelementptr inbounds %class.C, %class.C* [[OBJ]], i32 0, i32 1
// CHECK-DAG: [[BARRBEGIN:%.+]] = load double*, double** [[BBEGIN2]]
// CHECK-DAG: [[BARRBEGINGEP:%.+]] = getelementptr inbounds double, double* [[BARRBEGIN]], i[[sz]] 0
// CHECK-DAG: [[BEND:%.+]] = getelementptr double*, double** [[BBEGIN]], i32 1
// CHECK-DAG: [[ABEGINV:%.+]] = bitcast i32* [[ABEGIN]] to i8*
// CHECK-DAG: [[BENDV:%.+]] = bitcast double** [[BEND]] to i8*
// CHECK-DAG: [[ABEGINI:%.+]] = ptrtoint i8* [[ABEGINV]] to i64
// CHECK-DAG: [[BENDI:%.+]] = ptrtoint i8* [[BENDV]] to i64
// CHECK-DAG: [[CSIZE:%.+]] = sub i64 [[BENDI]], [[ABEGINI]]
// CHECK-DAG: [[CUSIZE:%.+]] = sdiv exact i64 [[CSIZE]], ptrtoint (i8* getelementptr (i8, i8* null, i32 1) to i64)
// CHECK-32-DAG: [[CUSIZE32:%.+]] = trunc i64 [[CUSIZE]] to i32
// CHECK-64-DAG: store i[[sz]] [[CUSIZE]], i[[sz]]* [[SIZEADDR0:%[^,]+]]
// CHECK-32-DAG: store i[[sz]] [[CUSIZE32]], i[[sz]]* [[SIZEADDR0:%[^,]+]]
// CHECK-DAG: [[SIZEADDR0]] = getelementptr inbounds [3 x i[[sz]]], [3 x i[[sz]]]* [[SIZEADDR:%[^,]+]], i32 0, i32 0
// CHECK-DAG: store %class.C* [[OBJ]], %class.C** [[BPTRADDR0BC:%[^,]+]]
// CHECK-DAG: [[BPTRADDR0BC]] = bitcast i8** [[BPTRADDR0:%.+]] to %class.C**
// CHECK-DAG: [[BPTRADDR0]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[LBPTRADDR:%[^,]+]], i32 0, i32 0
// CHECK-DAG: store i32* [[ABEGIN]], i32** [[PTRADDR0BC:%[^,]+]]
// CHECK-DAG: [[PTRADDR0BC]] = bitcast i8** [[PTRADDR0:%.+]] to i32**
// CHECK-DAG: [[PTRADDR0]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[PTRADDR:%[^,]+]], i32 0, i32 0
// CHECK-DAG: [[TYPETF:%.+]] = and i64 [[TYPE]], 3
// CHECK-DAG: [[ISALLOC:%.+]] = icmp eq i64 [[TYPETF]], 0
// CHECK-DAG: br i1 [[ISALLOC]], label %[[ALLOC:[^,]+]], label %[[ALLOCELSE:[^,]+]]
// CHECK-DAG: [[ALLOC]]
// CHECK-DAG: br label %[[TYEND:[^,]+]]
// CHECK-DAG: [[ALLOCELSE]]
// CHECK-DAG: [[ISTO:%.+]] = icmp eq i64 [[TYPETF]], 1
// CHECK-DAG: br i1 [[ISTO]], label %[[TO:[^,]+]], label %[[TOELSE:[^,]+]]
// CHECK-DAG: [[TO]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TOELSE]]
// CHECK-DAG: [[ISFROM:%.+]] = icmp eq i64 [[TYPETF]], 2
// CHECK-DAG: br i1 [[ISFROM]], label %[[FROM:[^,]+]], label %[[TYEND]]
// CHECK-DAG: [[FROM]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TYEND]]
// CHECK-DAG: [[PHITYPE0:%.+]] = phi i64 [ 32, %[[ALLOC]] ], [ 32, %[[TO]] ], [ 32, %[[FROM]] ], [ 32, %[[TOELSE]] ]
// CHECK-DAG: store i64 [[PHITYPE0]], i64* [[TYPEADDR0:%[^,]+]]
// CHECK-DAG: [[TYPEADDR0]] = getelementptr inbounds [3 x i64], [3 x i64]* [[TYPEADDR:%[^,]+]], i32 0, i32 0
// CHECK-DAG: [[BPTRADDR1:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[LBPTRADDR]], i32 0, i32 1
// CHECK-DAG: [[BPTRADDR1BC:%.+]] = bitcast i8** [[BPTRADDR1]] to %class.C**
// CHECK-DAG: store %class.C* [[OBJ]], %class.C** [[BPTRADDR1BC]]
// CHECK-DAG: [[PTRADDR1:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[PTRADDR]], i32 0, i32 1
// CHECK-DAG: [[PTRADDR1BC:%.+]] = bitcast i8** [[PTRADDR1]] to i32**
// CHECK-DAG: store i32* [[ABEGIN]], i32** [[PTRADDR1BC]]
// CHECK-DAG: [[SIZEADDR1:%.+]] = getelementptr inbounds [3 x i[[sz]]], [3 x i[[sz]]]* [[SIZEADDR]], i32 0, i32 1
// CHECK-DAG: store i[[sz]] 4, i[[sz]]* [[SIZEADDR1]]
// CHECK-DAG: [[TYPEADDR1:%.+]] = getelementptr inbounds [3 x i64], [3 x i64]* [[TYPEADDR]], i32 0, i32 1
// CHECK-DAG: [[TYPETF:%.+]] = and i64 [[TYPE]], 3
// CHECK-DAG: [[ISALLOC:%.+]] = icmp eq i64 [[TYPETF]], 0
// CHECK-DAG: br i1 [[ISALLOC]], label %[[ALLOC:[^,]+]], label %[[ALLOCELSE:[^,]+]]
// CHECK-DAG: [[ALLOC]]
// CHECK-DAG: br label %[[TYEND:[^,]+]]
// CHECK-DAG: [[ALLOCELSE]]
// CHECK-DAG: [[ISTO:%.+]] = icmp eq i64 [[TYPETF]], 1
// CHECK-DAG: br i1 [[ISTO]], label %[[TO:[^,]+]], label %[[TOELSE:[^,]+]]
// CHECK-DAG: [[TO]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TOELSE]]
// CHECK-DAG: [[ISFROM:%.+]] = icmp eq i64 [[TYPETF]], 2
// CHECK-DAG: br i1 [[ISFROM]], label %[[FROM:[^,]+]], label %[[TYEND]]
// CHECK-DAG: [[FROM]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TYEND]]
// 0x1,000,000,000; 0x1,000,000,001; 0x1,000,000,002; 0x1,000,000,003
// CHECK-DAG: [[TYPE1:%.+]] = phi i64 [ 281474976710656, %[[ALLOC]] ], [ 281474976710657, %[[TO]] ], [ 281474976710658, %[[FROM]] ], [ 281474976710659, %[[TOELSE]] ]
// CHECK-DAG: store i64 [[TYPE1]], i64* [[TYPEADDR1]]
// CHECK-DAG: [[BPTRADDR2:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[LBPTRADDR]], i32 0, i32 2
// CHECK-DAG: [[BPTRADDR2BC:%.+]] = bitcast i8** [[BPTRADDR2]] to double***
// CHECK-DAG: store double** [[BBEGIN]], double*** [[BPTRADDR2BC]]
// CHECK-DAG: [[PTRADDR2:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[PTRADDR]], i32 0, i32 2
// CHECK-DAG: [[PTRADDR2BC:%.+]] = bitcast i8** [[PTRADDR2]] to double**
// CHECK-DAG: store double* [[BARRBEGINGEP]], double** [[PTRADDR2BC]]
// CHECK-DAG: [[SIZEADDR2:%.+]] = getelementptr inbounds [3 x i[[sz]]], [3 x i[[sz]]]* [[SIZEADDR]], i32 0, i32 2
// CHECK-DAG: store i[[sz]] 16, i[[sz]]* [[SIZEADDR2]]
// CHECK-DAG: [[TYPEADDR2:%.+]] = getelementptr inbounds [3 x i64], [3 x i64]* [[TYPEADDR]], i32 0, i32 2
// CHECK-DAG: [[TYPETF:%.+]] = and i64 [[TYPE]], 3
// CHECK-DAG: [[ISALLOC:%.+]] = icmp eq i64 [[TYPETF]], 0
// CHECK-DAG: br i1 [[ISALLOC]], label %[[ALLOC:[^,]+]], label %[[ALLOCELSE:[^,]+]]
// CHECK-DAG: [[ALLOC]]
// CHECK-DAG: br label %[[TYEND:[^,]+]]
// CHECK-DAG: [[ALLOCELSE]]
// CHECK-DAG: [[ISTO:%.+]] = icmp eq i64 [[TYPETF]], 1
// CHECK-DAG: br i1 [[ISTO]], label %[[TO:[^,]+]], label %[[TOELSE:[^,]+]]
// CHECK-DAG: [[TO]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TOELSE]]
// CHECK-DAG: [[ISFROM:%.+]] = icmp eq i64 [[TYPETF]], 2
// CHECK-DAG: br i1 [[ISFROM]], label %[[FROM:[^,]+]], label %[[TYEND]]
// CHECK-DAG: [[FROM]]
// CHECK-DAG: br label %[[TYEND]]
// CHECK-DAG: [[TYEND]]
// 0x1,000,000,010; 0x1,000,000,011; 0x1,000,000,012; 0x1,000,000,013
// CHECK-DAG: [[TYPE2:%.+]] = phi i64 [ 281474976710672, %[[ALLOC]] ], [ 281474976710673, %[[TO]] ], [ 281474976710674, %[[FROM]] ], [ 281474976710675, %[[TOELSE]] ]
// CHECK-DAG: store i64 [[TYPE2]], i64* [[TYPEADDR2]]
// CHECK-DAG: [[ARGBPTR:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[LBPTRADDR]], i32 0, i32 0
// CHECK-DAG: [[ARGPTR:%.+]] = getelementptr inbounds [3 x i8*], [3 x i8*]* [[PTRADDR]], i32 0, i32 0
// CHECK-DAG: [[ARGSIZE:%.+]] = getelementptr inbounds [3 x i[[sz]]], [3 x i[[sz]]]* [[SIZEADDR]], i32 0, i32 0
// CHECK-DAG: [[ARGTYPE:%.+]] = getelementptr inbounds [3 x i64], [3 x i64]* [[TYPEADDR]], i32 0, i32 0
// CHECK: [[RES:%.+]] = call i32 @__tgt_target_data_mapper_nowait(i64 [[DID]], i32 3, i8** [[ARGBPTR]], i8** [[ARGPTR]], i[[sz]]* [[ARGSIZE]], i64* [[ARGTYPE]], i8** null)
// CHECK: [[ISERR:%.+]] = icmp ne i32 [[RES]], 0
// CHECK: br i1 [[ISERR]], label %[[LERR:[^,]+]], label %[[LCORRECT]]
// CHECK: [[LERR]]
// CHECK: store i32 [[RES]], i32* %retval
// CHECK: br label %[[DONE]]
// CHECK: [[LCORRECT]]
// CHECK: [[PTRNEXT]] = getelementptr %class.C*, %class.C** [[PTR]], i32 1
// CHECK: [[ISDONE:%.+]] = icmp eq %class.C** [[PTRNEXT]], [[PTREND]]
// CHECK: br i1 [[ISDONE]], label %[[LEXIT:[^,]+]], label %[[LBODY]]

// CHECK: [[LEXIT]]
// CHECK: br i1 [[ISARRAY]], label %[[EVALDEL:[^,]+]], label %[[DONE]]
// CHECK: [[EVALDEL]]
// CHECK: [[TYPEDEL:%.+]] = and i64 [[TYPE]], 8
// CHECK: [[ISDEL:%.+]] = icmp ne i64 [[TYPEDEL]], 0
// CHECK: br i1 [[ISDEL]], label %[[DEL:[^,]+]], label %[[DONE]]
// CHECK: [[DEL]]
// CHECK-64: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 16
// CHECK-32: [[ARRSIZE:%.+]] = mul i[[sz]] [[SIZE]], 8
// CHECK-DAG: store i[[sz]] [[ARRSIZE]], i[[sz]]* [[DSIZEADDR:%[^,]+]]
// CHECK-DAG: [[DSIZEADDR]] = getelementptr inbounds [1 x i[[sz]]], [1 x i[[sz]]]* [[DSIZE:%.+]], i32 0, i32 0
// CHECK-DAG: [[DTYPE:%.+]] = and i64 [[TYPE]], -4
// CHECK-DAG: store i64 [[DTYPE]], i64* [[DTYPEADDR:%[^,]+]]
// CHECK-DAG: [[DTYPEADDR]] = getelementptr inbounds [1 x i64], [1 x i64]* [[DTYPE:%.+]], i32 0, i32 0
// CHECK: [[DRES:%.+]] = call i32 @__tgt_target_data_mapper_nowait(i64 [[DID]], i32 1, i8** [[BPTRADDR]], i8** [[VPTRADDR]], i[[sz]]* [[DSIZEADDR]], i64* [[DTYPEADDR]], i8** null)
// CHECK: [[ISDELERR:%.+]] = icmp ne i32 [[DRES]], 0
// CHECK: br i1 [[ISDELERR]], label %[[DELERR:[^,]+]], label %[[DONE]]
// CHECK: [[DELERR]]
// CHECK: store i32 [[DRES]], i32* %retval
// CHECK: br label %[[DONE]]
// CHECK: [[DONE]]
// CHECK: [[RET:%.+]] = load i32, i32* %retval
// CHECK: ret i32 [[RET]]


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
