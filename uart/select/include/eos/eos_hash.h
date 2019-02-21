#ifndef __EOS_HASH_H
#define __EOS_HASH_H


#ifdef __cplusplus
extern "C" {
#endif

EOS_DECLARE_HANDLE(EOSHHASH);
EOS_DECLARE_HANDLE(HdEl);
EOS_DECLARE_HANDLE(Element);

typedef _UINT32 (*pfHashFunc) (IN _VOID *param, _INT32 paramSize, _INT32 hashSize);

typedef EOSBool (*pfHashCompareFunc) (IN _VOID *key1, IN _VOID* key2, IN _UINT32 keySize);

typedef _VOID*(*pfHashEFunc)(IN EOSHHASH hHash, IN _VOID* elem, _VOID *param);

/* 20060401    */
typedef EOSBool (*pfHashSearchCompFunc) (IN _VOID* pElement,IN _VOID* pData);


/* 20060411    v.020 */
typedef _VOID (*pfHashDoAllFunc) (IN _VOID* pElement,IN _VOID* pData);


_UINT32 eos_hsh_int(IN _VOID* param,IN _INT32 paramSize,IN _INT32 hashSize);
_UINT32 eos_hsh_str(IN _VOID*    param,IN _INT32    paramSize,IN _INT32    hashSize);
/* 20060411    for array hash default function    */
_UINT32 eos_hsh_array(IN _VOID* param,IN _INT32 paramSize,IN _INT32 hashSize);
EOSBool eos_hsh_default_compare(IN _VOID *key1, IN _VOID* key2, IN _UINT32 keySize);
/* young 20060704    */
EOSBool eos_hsh_str_compare(IN _VOID *key1, IN _VOID* key2, IN _UINT32 keySize);
EOSHHASH eos_hsh_construct(IN _INT32 numOfKeys,IN _INT32 numOfElems,IN pfHashFunc hashFunc,
    IN pfHashCompareFunc compareFunc,IN _INT32 keySize,
    IN _INT32 elemSize,IN __CONST__ _CHAR8* name);
_INT32    eos_hsh_destruct(IN  EOSHHASH hHash);
_VOID*    eos_hsh_add(IN  EOSHHASH  hHash,IN  _VOID*  key,IN  _VOID*  userElem,IN  EOSBool   searchBeforeInsert);
_INT32    eos_hsh_remove(IN  EOSHHASH pHash, IN _VOID* key, OUT _VOID* val);
_VOID*    eos_hsh_find(IN  EOSHHASH hHash,IN  _VOID* key);
 _VOID*    eos_hsh_find_next(IN  EOSHHASH hHash,IN  _VOID* key,IN  _VOID* location);
_VOID*    eos_hsh_get_element(IN  EOSHHASH hHash,IN  _VOID* location);
_VOID*    eos_hsh_get_key(IN  EOSHHASH hHash,IN  _VOID* location);
_INT32 eos_hsh_delete(IN  EOSHHASH   hHash,IN  _VOID*   location);
_INT32    eos_hsh_set_element(IN  EOSHHASH   hHash,IN  _VOID*   location,IN  _VOID*   userElem);
/* 20060608    */
_VOID* eos_hsh_search(IN     EOSHHASH hHash,IN _VOID* pKey,pfHashSearchCompFunc pCompareFunc,_VOID* pData,EOSBool bDel);
_INT32 eos_hsh_usage(IN EOSHHASH   hHash);
/* 20060425    v.020 */
_INT32 eos_hsh_doall(IN EOSHHASH hHash,IN pfHashDoAllFunc pfFunc,IN _VOID* pData);

_INT32 eos_hsh_size(IN EOSHHASH   hHash);
_INT32 eos_hsh_memsize(IN EOSHHASH   hHash);

#ifdef __cplusplus
}
#endif

#endif  /* _HASH_H */


