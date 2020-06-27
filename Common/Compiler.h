#ifndef _COMPILER_H_
#define _COMPILER_H_

/*****************************************************************************/
/* AUTOSAR Release: 4.3.1                                                      */
/*****************************************************************************/



#define _TEXAS_INSTRUMENTS_C_TMS470_
#define _TEXAS_INSTRUMENTS_C_ARM_

#define COMPILER_MAJOR_VERSION __TI_COMPILER_MAJOR_VERSION__
#define COMPILER_MINOR_VERSION __TI_COMPILER_MINOR_VERSION__
#define COMPILER_PATCH_VERSION __TI_COMPILER_PATCH_VERSION__

/*****************************************************************************/
/* Define:       AUTOMATIC                                                   */
/* Description:  Used for the declaration of local pointers                  */
/*****************************************************************************/
#define AUTOMATIC

/*****************************************************************************/
/* Define:       TYPEDEF                                                     */
/* Description:  The memory class shall be used within type definitions,     */
/*               where no memory qualifier can be specified                  */
/*****************************************************************************/
#define TYPEDEF

/*****************************************************************************/
/* Define:       NULL_PTR                                                    */
/* Description:  Void pointer to 0                                           */
/*****************************************************************************/
#define NULL_PTR ((void *)0x0)

/*****************************************************************************/
/* Define:       INLINE                                                      */
/* Description:  Abstraction of the keyword inline                           */
/*****************************************************************************/
#define INLINE __inline

/*****************************************************************************/
/* Define:       LOCAL_INLINE                                                */
/* Description:  Abstraction of keyword inline with static scope.            */
/*****************************************************************************/
#define LOCAL_INLINE static inline


/*****************************************************************************/
/* Define:       LOCAL 		                                                 */
/* Description:  Abstraction of keyword inline with static scope.            */
/*****************************************************************************/
#define LOCAL static

/*****************************************************************************/
/* Macro name: P2VAR                                                         */
/* Parameters: ptrtype     type of the referenced variable                   */
/*             memclass    classification of the pointer's variable itself   */
/*             ptrclass    defines the classification of the pointer's       */
/*                         distance                                          */
/*****************************************************************************/
#define P2VAR(ptrtype, memclass, ptrclass) ptrtype *

/*****************************************************************************/
/* Macro name: FUNC                                                          */
/* Parameters: rettype     return type of the function                       */
/*             memclass    classification of the function itself             */
/*****************************************************************************/
#define FUNC(rettype, memclass) rettype

/*****************************************************************************/
/* Macro name: P2VAR                                                         */
/* Parameters: ptrtype     type of the referenced variable                   */
/*             memclass    classification of the pointer's variable itself   */
/*             ptrclass    defines the classification of the pointer's       */
/*                         distance                                          */
/*****************************************************************************/
#define FUNC_P2VAR(ptrtype, memclass, ptrclass) ptrtype *


/*****************************************************************************/
/* Macro name: P2CONST                                                       */
/* Parameters: ptrtype     type of the referenced constant                   */
/*             memclass    classification of the pointer's varaible itself   */
/*             ptrclass    defines the classification of the pointer's       */
/*                         distance                                          */
/*****************************************************************************/
#define FUNC_P2CONST(ptrtype, memclass, ptrclass) const ptrtype *

/*****************************************************************************/
/* Macro name: CONSTP2VAR                                                    */
/* Parameters: ptrtype     type of the referenced variable                   */
/*             memclass    classification of the pointer's constant itself   */
/*             ptrclass    defines the classification of the pointer's       */
/*                         distance                                          */
/*****************************************************************************/
#define P2CONST(ptrtype, memclass, ptrclass) ptrtype * const



/*****************************************************************************/
/* Macro name: CONSTP2CONST                                                  */
/* Parameters: ptrtype     type of the referenced constant                   */
/*             memclass    classification of the pointer's constant itself   */
/*             ptrclass    defines the classification of the pointer's       */
/*                         distance                                          */
/*****************************************************************************/
#define CONSTP2CONST(ptrtype, memclass, ptrclass) const ptrtype * const

/*****************************************************************************/
/* Macro name: CONSTP2VAR                                                       */
/* Parameters: ptrtype     type of the referenced variable                   */
/*             memclass    classification of the pointer's constant itself   */
/*             ptrclass    defines the classification of the pointer's       */
/*                         distance                                          */
/*****************************************************************************/
#define CONSTP2VAR(ptrtype, memclass, ptrclass) const ptrtype *

/*****************************************************************************/
/* Macro name: P2FUNC                                                        */
/* Parameters: rettype     return type of the function                       */
/*             ptrclass    defines the classification of the pointer's       */
/*                         distance                                          */
/*             fctname     function name respectively name of the defined    */
/*                         type                                              */
/*****************************************************************************/
#define P2FUNC(rettype, ptrclass, fctname) rettype (*fctname)

/*****************************************************************************/
/* Macro name: CONST                                                         */
/* Parameters: consttype   type of the constant                              */
/*             memclass    classification of the constant itself             */
/*****************************************************************************/
#define CONST(consttype, memclass) const consttype

/*****************************************************************************/
/* Macro name: VAR                                                           */
/* Parameters: vartype     type of the variable                              */
/*             memclass    classification of the variable itself             */
/*****************************************************************************/
#define VAR(vartype, memclass) vartype




#endif /* _COMPILER_H_ */
