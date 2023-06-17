# vector-c
Vector w/ Internal Iterator in C. Calling `vector_free()` will free the underlying vector memory, but NOT any dynamically allocated pointers/items it may store--that needs to be done manually.

```C
/// Default item count for new vectors.
#define vector_DEFAULT_LENGTH 32

/// Vector with internal iterator that accepts void* (generic) data with byte-size typeSize.
typedef struct vector {
	void_t* data; // Data Pointer
	int32_t typeSize; // Type Size (Byte Length)
	size_t length;   // Current Size (Bytes, not Items)
	size_t iterator; // Current Iterator (Bytes, not Items)
} vector;

/// Returns a new vector (with memory allocated if reserved is TRUE) with default length vector_DEFAULT_LENGTH.
vector vector_calloc(int32_t typeSize, bool32_t reserve);
/// Returns a new vector (with memory allocated if reserved is TRUE).
vector vector_calloc2(int32_t typeSize, size_t length, bool32_t reserve);
/// Returns TRUE if the vector was free'd, else FALSE if the vector passed is not allocated.
void_t vector_free(vector* vector);
/// Attempts to resize the vector: Returns true if the vector is allocated (regardless if it was resized), else FALSE.
bool32_t vector_realloc(vector* vector, size_t length);
/// Returns TRUE if the vector is allocated, else FALSE.
bool32_t vector_isalloc(vector* vector);
/// Returns the full byte-length of allocated memory for a vector.
size_t vector_bytesize(vector* vector);
/// Returns the maximum number of elements that can be written (until resized on insert).
size_t vector_maxsize(vector* vector);
/// Returns the byte-size of the type used for allocations within a vector.
size_t vector_typesize(vector* vector);
/// Returns the last iterator position (same-as item count) of a vector.
size_t vector_last(vector* vector);
/// Returns the first position (0) of a vector. [Redundant, Placeholder]
size_t vector_first(vector* vector);
/// Returns FALSE if [iterator] is not within bounds length > iterator >= 0, else TRUE and set new iterator position.
bool32_t vector_move(vector* vector, size_t iterator);
/// Returns TRUE if the vector can be cleared, else FALSE.
bool32_t vector_clear(vector* vector, void_t* data);
/// Returns TRUE if the vector is allocated and the element at [index] can be written, else FALSE.
bool32_t vector_insert(vector* vector, void_t* data, size_t index);
/// Returns TRUE if the item replaces an existing item in the vector, else FALSE.
bool32_t vector_replace(vector* vector, void_t* data, size_t index);
/// Returns TRUE if the item replaces an existing item in the vector, else FALSE.
bool32_t vector_replace_unsafe(vector* vector, void_t* data, size_t index, size_t byteCount);
/// Returns TRUE if the element at [index] can be removed, else FALSE.
bool32_t vector_remove(vector* vector, size_t index);
/// Returns NULL if index is not within bounds iterator > index >= 0 or returns pointer to element in vector.
void_t* vector_get(vector* vector, size_t index);
/// Returns a pointer to a new string constructor from a vector: outLen can be pointer to get length, or NULL to ignore.
char_t* vector_makestr(vector* vector, size_t first, size_t last, size_t* outLen);
/// Creates a calloc'd copy of the passed string.
char_t* vector_cpystr(const char_t* str)
```
