.onLoad = function(libname, pkgname) {
	print("===== RIOT loaded ===== ")
# check for any dbvector in .GlobalEnv
# construct proper ext ptr for rdbVector info structure
	#for(i in ls(.GlobalEnv)) {
		#if(class(get(i))=="dbvector") {
			#.Call("initInfo", get(i))
		#}
	#}


	
	setClass("riotvector", representation(ptr="externalptr"))


 	setMethod("show", signature("riotvector"), show_riotvector)
 	setMethod("length", signature("riotvector"), length_riotvector)
	setMethod("+", signature(e1="riotvector", e2="riotvector"), add_riotvectors)
	setMethod("-", signature(e1="riotvector", e2="riotvector"), subtract_riotvectors)
	setMethod("-", signature(e1="riotvector", e2="numeric"), subtract_riotvector_numeric)
	setMethod("*", signature(e1="riotvector", e2="riotvector"), multiply_riotvectors)
	setMethod("/", signature(e1="riotvector", e2="riotvector"), divide_riotvectors)
	setMethod("^", signature(e1="riotvector", e2="numeric"), pow_riotvectors)
 	setMethod("sqrt", signature(x="riotvector"), sqrt_riotvector)
 	setMethod("[", signature(x="riotvector"), function(x,i,j,...,drop=T){
 		.Call("indexRiotVector", x, i)})
 	setReplaceMethod("[", "riotvector", function(x,i,j,...,value){
	    .Call("subsetAssign", x, i, value);
	})



	setClass("riotmatrix", contains="riotvector")
	setMethod("dim", signature("riotmatrix"), function(x){.Call("getDim",x)})
}

	set.mem <- function(m=mm, k=mk) {
			if (!missing(mm))
			return(.Call("setMem", mm, 2))
			if (!missing(mk))
			return(.Call("setMem", mk, 1))			
	}
	
	vector.riot <- function(x) {
	if (class(x)!='numeric' && class(x)!='integer')
	   stop('riot vector can only be initialized by	a numeric vector')
	.Call('createRiotVector', x)
	}

	length_riotvector <- function(object) {
	  .Call('lengthRiotVector', object)
	}
	
	show_riotvector <- function(object) {
	  .Call('showRiotVector', object)
	}

	add_riotvectors <- function(e1, e2) {
		.Call('addRiotVectors', e1, e2)
	}

	subtract_riotvectors <- function(e1, e2) {
		.Call('subtractRiotVectors', e1, e2)
	}
	subtract_riotvector_numeric <- function(e1, e2) {
		.Call('subtractRiotVectorNumeric', e1, e2)
	}
	multiply_riotvectors <- function(e1, e2) {
		.Call('multiplyRiotVectors', e1, e2)
	}
	divide_riotvectors <- function(e1, e2) {
		.Call('divideRiotVectors', e1, e2)
	}
	pow_riotvectors <- function(e1,e2) {
		.Call('powRiotVectors', e1, e2)
	}
	sqrt_riotvector <- function(x) {
		.Call('sqrtRiotVector', x)
	}

# 	setClass("dbmatrix", representation(size="numeric",type="character",tablename="character",info="raw"))
# 	setMethod("show", signature("dbmatrix"), show.dbmatrix)
# 	setMethod("+", signature(e1="dbvector",e2="dbvector"), add_dbvectors)
# 	setMethod("-", signature(e1="dbvector",e2="dbvector"), subtract_dbvectors)
# 	setMethod("*", signature(e1="dbvector",e2="dbvector"), multiply_dbvectors)
# 	setMethod("/", signature(e1="dbvector",e2="dbvector"), divide_dbvectors)
# 	setMethod("Compare", signature(e1="dbvector",e2="numeric"), compare_dbvector_numeric)
# 	setMethod("Math", signature(x="dbvector"), math_dbvector)
# 	setMethod("^", signature(e1="dbvector",e2="numeric"), pow_dbvector)
# 	setMethod("-", signature(e1="dbvector",e2="numeric"), subtract_dbvector_numeric)
	
# 	setMethod("dim", signature("dbmatrix"), function(x) {.Call("get_dim",x)})
# #setMethod("dim<-", signature("dbmatrix"), function(x,d) {
# 	setMethod("%*%", signature(x="dbmatrix", y="dbmatrix"), function(x,y){
# 		if (dim(x)[2]!=dim(y)[1])
# 		stop("dimensions are not conformable")
# 		.Call("multiply_matrices", x, y)
# 		})

# 	setMethod("max", signature(x="dbvector"), function(x,na.rm=FALSE){
# 		.Call("max_dbvector", x, na.rm)	
# 	})
# 	setMethod("min", signature(x="dbvector"), function(x,na.rm=FALSE){
# 		.Call("min_dbvector", x, na.rm)	
# 	})
# 	setMethod("sum", signature(x="dbvector"), function(x,na.rm=FALSE){
# 		.Call("sum_dbvector", x, na.rm)	
# 	})
# 	#setMethod("mean", signature(x="dbvector"), function(x,trim=0,na.rm=FALSE,...){
# 		#.Call("mean_dbvector", x, na.rm)	
# 	#})


# 	setMethod("[", signature(x="dbmatrix"), function(x,i,j,...,drop=T){
# 		# for matrix only take the first two
# 			if(missing(i)||missing(j))
# 			stop("must provide two indexes")
# 		.Call("get_matrix_by_index", x, i, j)})

# 		nas <- is.na(value)
# 		# if value contains only NA, delete instead
# 		if (all(nas)) {
# 			 # currently i must of of numeric type, no dbvector!
# 			 # sort index so that deletion can be done in ranges
# 			return(.Call("delete_from_dbvector", x, sort(i)))
# 		}
# 		# if some inside value are NA, first set then delete
# 		if (any(nas)) {
# 			entireNA <- rep(nas,length.out=length(i))
# 			toDel <- i[entireNA]
# 			y <- .Call("set", x, i, value)
# 			return(.Call("delete_from_dbvector", y, toDel))
# 		}
# 		 # no NA's
# 		.Call("set", x, i, value)
# 	})
# }

# sqrt_dbvector <- function(x) {
# .Call("sqrt_dbvector", x)
# }
# pow_dbvector <- function(e1,e2) {
# .Call("pow_dbvector", e1, e2)
# }
# mean.dbvector <- function(x, na.rm=F) {
# 	.Call("mean_dbvector", x, na.rm)
# }

# compare_dbvector_numeric <- function(e1, e2) {
# # a hack for SQL
# 	if (.Generic == "==") .Generic = "="
# 	.Call("compare_dbvector_numeric", e1, e2, .Generic)
# }

# math_dbvector <- function(x) {
# 	switch(.Generic,
# 		sin=,
# 		cos= .Call("math_dbvector", x, .Generic)
# 		)
# }

# matrix.db <- function(nrows, ncols, from, to, by=1) {
# #if (missing(nrows)||missing(ncols)||missing(from)||missing(to))
# #		stop("arguments misssing")
# 	.Call("dbmatrix_from_seq", nrows, ncols, from, to, by)
# }

# seq.db <- function(from=1, to=1, by=1) {
# 	if (missing(to) && missing(by)) {
# 		# copy the regular vector to a dbvector
# 		.Call("dbvector_copy", from)
# 	}
# 	else {
# 	# begin and end are always doubles 
# 	if (!is.real(by)) stop("by should be an integer or real")
# 	if (to < from && missing(by)) by=-1
# 	if ((to < from && by >= 0) || (to > from && by <=0)) stop("wrong sign of by")
	
# 	.Call("dbvector_from_seq", from, to, by)
# 	}
# }

# complex.db <- function(length.out=0, real=numeric(), imaginary=numeric(),modulus=1,argument=0) {
# 	if (missing(modulus) && missing(argument))
# 		.Call("dbcomplex",length.out, real, imaginary, 0)
# 	else {
# 		n <- max(length.out, length(argument), length(modulus))
# 		temp <- rep(modulus,n)*exp((0+1i)*rep(argument,n))
# 		.Call("dbcomplex",n,Re(temp), Im(temp), 0)
# 	}
# }

# logical.db <- function(length=1, value=FALSE) {
# 	if (length < 1) stop("length must be at least 1")
# 	.Call("logical_db", length, value)
# }

# length.dbvector <- function(object) {
# 	.Call("length_dbvector", object)
# }

# show.dbvector <- function(object) {
# 	.Call("show_dbvector", object)
# }

# show.dbmatrix <- function(object) {
# 	.Call("show_dbmatrix", object)
# }
# add_dbvectors <- function(e1,e2) {
# 	.Call("add_dbvectors", e1, e2)
# }
# subtract_dbvectors <- function(e1,e2) {
# 	.Call("subtract_dbvectors", e1, e2)
# }
# multiply_dbvectors <- function(e1,e2) {
# 	.Call("multiply_dbvectors", e1, e2)
# }
# divide_dbvectors <- function(e1,e2) {
# 	.Call("divide_dbvectors", e1, e2)
# }

# sort.dbvector <- function(x, decreasing=FALSE) {
# print("sort entered")
# .Call("dbvector_sort", x, decreasing)
# }

# load.dbvector <- function(name) {
# 	if (missing(name) || mode(name)!='numeric')
# 		stop("table metadata ID must be provided")
# 	.Call("load_dbvector", as.integer(name))
# }

# load.dbmatrix <- function(name) {
# 	if (missing(name) || mode(name)!='numeric')
# 		stop("table metadata ID must be provided")
# 	.Call("load_dbmatrix", as.integer(name))
# }
# persist <- function(x) {
# 	if (class(x)=="dbvector")
# 	.Call("persist_dbvector",x)
# 	else if (class(x)=="dbmatrix")
# 	.Call("persist_dbmatrix",x)
# }

# subtract_dbvector_numeric <- function(e1,e2) {
# if (length(e2) ==1)
# 	.Call("subtract_dbvector_numeric", e1, e2)
# else stop("second argument must be of length 1")
# }

# materialize <- function(x) {
#     if (class(x)=="dbvector")
#         .Call("materialize_dbvector", x)
#     else if(class(x)=="dbmatrix")
#         .Call("materialize_dbmatrix", x)
# 
