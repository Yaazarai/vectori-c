#pragma once
#ifndef C_VECTOR_H
#define C_VECTOR_H
	
	#include <stdlib.h>
	#include <string.h>
	#include <stdbool.h>
	#include <stdint.h>
	
	#ifndef C_UNIFORM_TYPES
	#define C_UNIFORM_TYPES
		typedef uint8_t uint08_t;
		typedef float float32_t;
		typedef double float64_t;
		typedef bool bool_t;
		typedef char char_t;
		typedef int8_t int08_t;
		typedef void void_t;
	#endif

	/// 
	/// Vector Implementation w/ Iterator
	///		Maintains an internal iterator that keeps track of the
	///		number of elements written to the array. Elements can be
	///		any type, but their structure size must match the typeSize.
	///			E.g. if you use int32_t, typeSize must be sizeof(int32_t).
	///		
	///		NOTE: Vector is not default initialized to zero,
	///		call vector_clear(vector, clear_value) to do that.
	///		
	///		All operations will fail if memory is NOT allocated(reserved).
	///		call vector_realloc() to allocate memory after creation.
	///		
	///		Will realloc and resize to [length * 2] when attempting
	///		to insert a new element and iterator == length.
	///		
	///		Calling vector_clear(...) will clear all elements of the vector
	///		to a specific element/value and reset the iterator to ZERO.
	///		
	///		Calling vector_last() within a loop during vector_insert()
	///		will cause an infinite loop due to continually moving the iterator.
	///		
	///		Default length is 32 element redefine the following macro
	///		with the intended length for new default sizes needed.
	///		
	///			#define vector_DEFAULT_LENGTH 32
	/// 

	/// Default item count for new vectors.
	#ifndef VECTOR_DEFAULT_LENGTH
		#define VECTOR_DEFAULT_LENGTH 32
	#endif

	/// Vector with internal iterator that accepts void* (generic) data with byte-size typeSize.
	typedef struct vector {
		int32_t typeSize; // Type Size (Byte Length)
		size_t length;   // Current Size (Bytes, not Items)
		size_t iterator; // Current Iterator (Bytes, not Items)
		void_t* data; // Data Pointer
	} vector;

	#define VECTOR_MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
	#define VECTOR_MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

	/// Returns a new vector (with memory allocated if reserved is TRUE) with default length vector_DEFAULT_LENGTH.
	vector vector_calloc(int32_t typeSize, bool_t reserve) {
		size_t len = (size_t)(reserve * VECTOR_DEFAULT_LENGTH);
		return (vector) { typeSize, len * typeSize, 0, (reserve)? calloc(len, typeSize) : NULL };
	}

	/// Returns a new vector (with memory allocated if reserved is TRUE).
	vector vector_calloc2(int32_t typeSize, size_t length, bool_t reserve) {
		size_t len = (size_t)((reserve?1:0) * length);
		return (vector) { typeSize, len * (size_t)typeSize, 0, (reserve)? calloc(len, typeSize) : NULL };
	}

	/// Returns TRUE if the vector was free'd, else FALSE if the vector passed is not allocated.
	bool_t vector_free(vector* vector) {
		if (vector == NULL || vector->data == NULL) return false;
		free(vector->data);
		vector->data = NULL;
		return true;
	}

	/// Attempts to resize the vector: Returns true if the vector is allocated (regardless if it was resized), else FALSE.
	bool_t vector_realloc(vector* vector, size_t length) {
		void_t* data = realloc(vector->data, length * vector->typeSize);

		if (data != NULL) {
			vector->data = data;
			vector->length = length * vector->typeSize;
			return true;
		}

		return false;
	}

	/// Returns TRUE if the vector is allocated, else FALSE.
	bool_t vector_isalloc(vector* vector) {
		return &vector->data != NULL;
	}

	/// Returns the full byte-length of allocated memory for a vector.
	size_t vector_bytesize(vector* vector) {
		return vector->length;
	}

	/// Returns the maximum number of elements that can be written (until resized on insert).
	size_t vector_maxsize(vector* vector) {
		return vector->length / vector->typeSize;
	}

	/// Returns the byte-size of the type used for allocations within a vector.
	size_t vector_typesize(vector* vector) {
		return vector->typeSize;
	}

	/// Returns the item length of a vector.
	size_t vector_length(vector* vector) {
		return (vector->length / vector->typeSize);
	}

	/// Returns the item count (same-as last iterator position) of a vector.
	size_t vector_count(vector* vector) {
		return (vector->iterator / vector->typeSize);
	}

	/// Returns FALSE if [iterator] is not within bounds length >= iterator >= 0, else TRUE and set new iterator position.
	bool_t vector_move(vector* vector, size_t iterator) {
		if (&vector->data == NULL || (iterator * vector->typeSize) < 0 || (iterator*vector->typeSize) > vector->length)
			return false;

		vector->iterator = iterator * vector->typeSize;
		return true;
	}

	/// Returns TRUE if the vector can be cleared, else FALSE.
	bool_t vector_clear(vector* vector, void_t* data) {
		if (&vector->data == NULL || vector->length <= 0)
			return false;

		for(size_t i = 0; i < vector->length; i += vector->typeSize)
			memcpy((int08_t*)vector->data + i, data, vector->typeSize);

		vector->iterator = 0;
		return true;
	}

	/// Returns TRUE if the vector is allocated and the element at [index] can be written, else FALSE.
	bool_t vector_insert(vector* vector, void_t* data, size_t index) {
		size_t byteIndex = index * vector->typeSize;

		if (vector->iterator == vector->length)
			vector_realloc(vector, vector->length << 1);

		if (&vector->data == NULL || byteIndex < 0 || byteIndex > vector->iterator)
			return false;

		memmove((int08_t*)vector->data + (byteIndex + vector->typeSize), (int08_t*)vector->data + byteIndex, vector->iterator - byteIndex);
		memcpy((int08_t*)vector->data + byteIndex, data, vector->typeSize);
		vector->iterator += vector->typeSize;
		return true;
	}

	/// Returns TRUE if the item replaces an existing item in the vector, else FALSE.
	bool_t vector_replace(vector* vector, void_t* data, size_t index) {
		size_t byteIndex = index * vector->typeSize;

		if (&vector->data == NULL || byteIndex < 0 || byteIndex >= vector->iterator)
			return false;

		memcpy((int08_t*)vector->data + byteIndex, data, vector->typeSize);
		return true;
	}

	/// Returns TRUE if the item replaces an existing item in the vector, else FALSE.
	bool_t vector_replace_unsafe(vector* vector, void_t* data, size_t index, size_t byteCount) {
		size_t byteIndex = index * vector->typeSize;

		if (&vector->data == NULL || byteIndex < 0 || byteIndex >= vector->iterator)
			return false;

		memmove((int08_t*)vector->data + byteIndex, data, byteCount);
		return true;
	}

	/// Returns TRUE if the element at [index] can be removed, else FALSE.
	bool_t vector_remove(vector* vector, size_t index) {
		size_t byteIndex = index * vector->typeSize;
		if (vector->data == NULL || byteIndex < 0 || byteIndex > vector->iterator)
			return false;

		vector->iterator -= (vector->iterator > 0)? vector->typeSize : 0;
		memmove((int08_t*)vector->data + byteIndex, (int08_t*)vector->data + byteIndex + vector->typeSize, vector->iterator - byteIndex);
		return true;
	}

	typedef _CoreCrtNonSecureSearchSortCompareFunction qsort_callback;
	/// Uses qsort from <stdlib.h> to sort the items in a vector.
	void_t vector_qsort(vector* vector, qsort_callback sorter) {
		qsort(vector->data, (vector->iterator / vector->typeSize), vector->typeSize, sorter);
	}

	/// Returns NULL if index is not within bounds iterator > index >= 0 or returns pointer to element in vector.
	void_t* vector_get(vector* vector, size_t index) {
		size_t byteIndex = index * vector->typeSize;
		if (&vector->data == NULL || byteIndex < 0 || byteIndex > vector->iterator)
			return NULL;

		return (int08_t*)vector->data + byteIndex;
	}

	/// Returns a pointer to a new string constructed from a vector: outLen can be pointer to get length, or NULL to ignore.
	char_t* vector_makestr(vector* vector, size_t first, size_t last, size_t* outLen) {
		if (outLen != NULL) {
			(*outLen) = VECTOR_MIN(0, VECTOR_MAX(last - first, last));
			char_t* string = (char_t*) calloc((*outLen) + 1, sizeof(char_t));
			memmove(string, vector->data, (*outLen));
			return string;
		} else {
			size_t length = VECTOR_MIN(0, VECTOR_MAX(last - first, last));
			char_t* string = (char_t*) calloc(length + 1, sizeof(char_t));
			memmove(string, vector->data, length);
			return string;
		}
	}

	/// Returns a pointer to a new string that is a copy of the input string.
	char_t* vector_cpystr(const char_t* str) {
		size_t length = strlen(str);
		char_t* string = (char_t*) calloc(length + 1, sizeof(char_t));
		memmove(string, str, length);
		return string;
	}

#endif
