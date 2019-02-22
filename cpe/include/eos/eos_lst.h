#ifndef __EOS_LIST_H
#define __EOS_LIST_H


#ifdef __cplusplus
extern "C" {
#endif


typedef struct    tagNode        
{
    struct tagNode *pstNext;
    struct tagNode *pstPrev;
} _NODE,*_PNODE;

typedef struct    tagList        
{
    _NODE stNode;            
    size_t iCount;        
} _LIST,*_PLIST;

typedef    _VOID (*EOS_LSTFREE_FUNC)(_VOID* pAddr);

/* 20060308    */
typedef EOSBool (*pfLstCompareFunc) (_NODE * pNode, const _VOID* pKey);

/* 20060322    */
typedef _INT32 (*pfLstForEachProcFunc) (_NODE * pNode, const _VOID* pData);


_NODE*    eos_lst_first (_LIST *pList);
_NODE*    eos_lst_get (_LIST *pList);
_NODE*    eos_lst_last (_LIST *pList);
_NODE*    eos_lst_nstep (_NODE *pNode, int nStep);
_NODE*    eos_lst_next (_NODE *pNode);
_NODE*    eos_lst_nth(_LIST *pList, int nodenum);
_NODE*    eos_lst_prev(_NODE *pNode);
_INT32     eos_lst_count(_LIST *pList);
_INT32     eos_lst_find(_LIST *pList, _NODE *pNode);
_VOID     eos_lst_add(_LIST *pList, _NODE *pNode);
_VOID     eos_lst_concat(_LIST *pDstList, _LIST *pAddList);
_VOID     eos_lst_del(_LIST *pList, _NODE *pNode);
_VOID     eos_lst_extract(_LIST *pSrcList, _NODE *pStartNode, _NODE *pEndNode,_LIST *pDstList);
_VOID     eos_lst_init(_LIST *pList);
_VOID     eos_lst_insert(_LIST *pList, _NODE *pPrev, _NODE *pNode);
_INT32    eos_lst_uninit(_LIST *pList,EOS_LSTFREE_FUNC pFreeFunc);
/*     20060308    */
_NODE * eos_lst_search(_LIST *pList, const _VOID* pKey,pfLstCompareFunc compareFunc);
/* 20060322    */
_INT32 eos_lst_foreach(_LIST* pList,pfLstForEachProcFunc pFunc,_VOID* pData);
/* 20060325    */
_VOID eos_lst_addfront(_LIST *pList,_NODE *pNode);
_VOID eos_lst_addtail(_LIST *pList,_NODE *pNode);
_NODE * eos_lst_getfront(_LIST *pList);
_NODE * eos_lst_gettail(_LIST *pList);

#ifdef __cplusplus
}
#endif
#endif
