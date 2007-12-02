/*	Public domain	*/

#ifndef _AGAR_OBJECT_H_
#define _AGAR_OBJECT_H_

#define AG_OBJECT_TYPE_MAX 128
#define AG_OBJECT_NAME_MAX 128
#define AG_OBJECT_PATH_MAX 1024
#define AG_OBJECT_DIGEST_MAX 170

#define AGOBJECT(ob) ((struct ag_object *)(ob))

#ifdef _AGAR_INTERNAL
#include <core/timeout.h>
#include <core/prop.h>
#else
#include <agar/core/timeout.h>
#include <agar/core/prop.h>
#endif

#include "begin_code.h"

struct ag_event;

/* Generic object operation vector */
typedef struct ag_object_class {
	const char *name;			/* Class name */
	size_t size;				/* Structure size */
	AG_Version ver;				/* Version numbers */

	void (*init)(void *);
	void (*reinit)(void *);
	void (*destroy)(void *);
	int (*load)(void *, AG_DataSource *, const AG_Version *); 
	int (*save)(void *, AG_DataSource *);
	void *(*edit)(void *);
} AG_ObjectClass;

/* Dependency with respect to another object. */
typedef struct ag_object_dep {
	char		 *path;		/* Unresolved object path */
	struct ag_object *obj;		/* Resolved object */
	Uint32		  count;	/* Reference count */
#define AG_OBJECT_DEP_MAX (0xffffffff-2) /* If reached, stay resident */
	AG_TAILQ_ENTRY(ag_object_dep) deps;
} AG_ObjectDep;

AG_TAILQ_HEAD(ag_objectq, ag_object);

typedef struct ag_object {
	char name[AG_OBJECT_NAME_MAX];	/* Object ID (unique in parent) */
	char *archivePath;		/* Path to archive (app-specific) */
	char *save_pfx;			/* Prefix for default save paths */
	AG_ObjectClass *cls;		/* Object class data */
	Uint flags;
#define AG_OBJECT_RELOAD_PROPS	 0x0001	/* Don't free props before load */
#define AG_OBJECT_NON_PERSISTENT 0x0002	/* Never include in saves */
#define AG_OBJECT_INDESTRUCTIBLE 0x0004	/* Not destructible (advisory) */
#define AG_OBJECT_RESIDENT	 0x0008	/* Data part is resident */
#define AG_OBJECT_PRESERVE_DEPS	 0x0010	/* Preserve cnt=0 dependencies */
#define AG_OBJECT_STATIC	 0x0020	/* Don't free() after detach */
#define AG_OBJECT_READONLY	 0x0040	/* Disallow edition (advisory) */
#define AG_OBJECT_WAS_RESIDENT	 0x0080	/* Used internally by ObjectLoad() */
#define AG_OBJECT_REOPEN_ONLOAD	 0x0200	/* Recreate editor UI on ObjectLoad() */
#define AG_OBJECT_REMAIN_DATA	 0x0400	/* Keep user data resident */
#define AG_OBJECT_DEBUG		 0x0800	/* Enable debugging */
#define AG_OBJECT_NAME_ONATTACH	 0x1000	/* Generate name on attach */
#define AG_OBJECT_SAVED_FLAGS	(AG_OBJECT_RELOAD_PROPS|\
 				 AG_OBJECT_INDESTRUCTIBLE|\
				 AG_OBJECT_PRESERVE_DEPS|\
				 AG_OBJECT_READONLY|\
				 AG_OBJECT_REOPEN_ONLOAD|\
				 AG_OBJECT_REMAIN_DATA|\
				 AG_OBJECT_DEBUG)
#define AG_OBJECT_DUPED_FLAGS	(AG_OBJECT_SAVED_FLAGS|\
				 AG_OBJECT_NON_PERSISTENT|\
				 AG_OBJECT_REOPEN_ONLOAD|\
				 AG_OBJECT_REMAIN_DATA)

	AG_Mutex lock;
	Uint nevents;				/* Number of event handlers */
	AG_TAILQ_HEAD(,ag_event) events;	/* Event handlers */
	AG_TAILQ_HEAD(,ag_prop) props;		/* Generic property table */
	AG_CIRCLEQ_HEAD(,ag_timeout) timeouts;	/* Timers tied to object */

	/* Uses linkage_lock */
	AG_TAILQ_HEAD(,ag_object_dep) deps; /* Object dependencies */
	struct ag_objectq children;	 /* Child objects */
	void *parent;			 /* Back reference to parent object */
	AG_TAILQ_ENTRY(ag_object) cobjs; /* Entry in child object queue */
	AG_TAILQ_ENTRY(ag_object) tobjs; /* Entry in timeout queue */
} AG_Object;

enum ag_object_checksum_alg {
	AG_OBJECT_MD5,
	AG_OBJECT_SHA1,
	AG_OBJECT_RMD160
};

#define AGOBJECT_RESIDENT(ob)	(AGOBJECT(ob)->flags & AG_OBJECT_RESIDENT)
#define AGOBJECT_PERSISTENT(ob) ((AGOBJECT(ob)->flags & \
				 AG_OBJECT_NON_PERSISTENT) == 0)
#define AGOBJECT_DEBUG(ob)	(AGOBJECT(ob)->flags & AG_OBJECT_DEBUG)

#define AGOBJECT_INITIALIZER(ob, name, pfx, cls) {		\
		(name),(pfx),(cls),0,				\
		AG_MUTEX_INITIALIZER,				\
		NULL, 0, 0, 					\
		AG_TAILQ_HEAD_INITIALIZER((ob)->events),	\
		AG_TAILQ_HEAD_INITIALIZER((ob)->props),		\
		AG_CIRCLEQ_HEAD_INITIALIZER((ob)->timeouts),	\
		AG_TAILQ_HEAD_INITIALIZER((ob)->deps),		\
		AG_TAILQ_HEAD_INITIALIZER((ob)->children),	\
		NULL						\
	}

#define AGOBJECT_FOREACH_CHILD(var, ob, t)				\
	for((var) = (struct t *)AG_TAILQ_FIRST(&AGOBJECT(ob)->children); \
	    (var) != (struct t *)AG_TAILQ_END(&AGOBJECT(ob)->children); \
	    (var) = (struct t *)AG_TAILQ_NEXT(AGOBJECT(var), cobjs))

#define AGOBJECT_FOREACH_CLASS(var, ob, t, subclass)			\
	AGOBJECT_FOREACH_CHILD(var,ob,t)				\
		if (!AG_ObjectIsClass(var,(subclass))) {		\
			continue;					\
		} else

#define AGOBJECT_FOREACH_CHILD_REVERSE(var, ob, t)			\
	for((var) = (struct t *)AG_TAILQ_LAST(&AGOBJECT(ob)->children, \
	    ag_objectq); \
	    (var) != (struct t *)AG_TAILQ_END(&AGOBJECT(ob)->children); \
	    (var) = (struct t *)AG_TAILQ_PREV(AGOBJECT(var), ag_objectq, \
	    cobjs))

#ifdef _AGAR_INTERNAL
#define OBJECT(ob)		AGOBJECT(ob)
#define OBJECT_RESIDENT(ob)	AGOBJECT_RESIDENT(ob)
#define OBJECT_PERSISTENT(ob)	AGOBJECT_PERSISTENT(ob)
#define OBJECT_DEBUG(ob) 	AGOBJECT_DEBUG(ob)
#define OBJECT_INITIALIZER(obj,n,pfx,cls) \
	AGOBJECT_INITIALIZER((obj),(n),(pfx),(cls))
#define OBJECT_FOREACH_CHILD(var,ob,t) \
	AGOBJECT_FOREACH_CHILD((var),(ob),t)
#define OBJECT_FOREACH_CHILD_REVERSE(var,ob,t) \
	AGOBJECT_FOREACH_CHILD_REVERSE((var),(ob),t)
#define OBJECT_FOREACH_CLASS(var,ob,t,subclass) \
	AGOBJECT_FOREACH_CLASS((var),(ob),(t),(subclass))
#endif /* _AGAR_INTERNAL */

#define AG_ObjectLock(ob) AG_MutexLock(&(ob)->lock)
#define AG_ObjectUnlock(ob) AG_MutexUnlock(&(ob)->lock)

__BEGIN_DECLS
extern AG_ObjectClass agObjectClass;

void	*AG_ObjectNew(void *, const char *, AG_ObjectClass *);
void	 AG_ObjectAttach(void *, void *);
int	 AG_ObjectAttachToNamed(const char *, void *);
void	 AG_ObjectDetach(void *);
void	 AG_ObjectMove(void *, void *);

void	 AG_ObjectInit(void *, void *);
void	 AG_ObjectInitStatic(void *, void *);
void	 AG_ObjectFreeDataset(void *);
void	 AG_ObjectRemain(void *, int);
int	 AG_ObjectCopyName(const void *, char *, size_t)
	                   BOUNDED_ATTRIBUTE(__string__, 2, 3);
int	 AG_ObjectCopyDirname(const void *, char *, size_t)
	                      BOUNDED_ATTRIBUTE(__string__, 2, 3);
int	 AG_ObjectCopyFilename(const void *, char *, size_t)
	                       BOUNDED_ATTRIBUTE(__string__, 2, 3);
size_t	 AG_ObjectCopyChecksum(const void *, enum ag_object_checksum_alg,
	                       char *);
int	 AG_ObjectCopyDigest(const void *, size_t *, char *);
int	 AG_ObjectChanged(void *);
int	 AG_ObjectChangedAll(void *);

void		*AG_ObjectFind(const char *);
void		*AG_ObjectFindF(const char *, ...)
		                FORMAT_ATTRIBUTE(printf, 1, 2);
int		 AG_ObjectInUse(const void *);
void		 AG_ObjectSetName(void *, const char *, ...)
		                  FORMAT_ATTRIBUTE(printf, 2, 3);
void		 AG_ObjectSetArchivePath(void *, const char *);
void		 AG_ObjectGetArchivePath(void *, char *, size_t)
		                         BOUNDED_ATTRIBUTE(__string__, 2, 3);
void		 AG_ObjectSetClass(void *, void *);

int	 AG_ObjectIsClassGeneral(const AG_Object *, const char *);
int	 AG_ObjectGetInheritHier(void *, AG_ObjectClass ***, int *);

void	 AG_ObjectMoveUp(void *);
void	 AG_ObjectMoveDown(void *);
void	*AG_ObjectDuplicate(void *, const char *);
void	 AG_ObjectDestroy(void *);
void	 AG_ObjectUnlinkDatafiles(void *);
void	 AG_ObjectSetSavePfx(void *, char *);

void	 AG_ObjectFreeChildren(AG_Object *);
void	 AG_ObjectFreeProps(AG_Object *);
void 	 AG_ObjectFreeEvents(AG_Object *);
void	 AG_ObjectFreeDeps(AG_Object *);
void	 AG_ObjectFreeDummyDeps(AG_Object *);
void 	 AG_ObjectCancelTimeouts(void *, Uint);

int	 AG_ObjectPageIn(void *);
int	 AG_ObjectPageOut(void *);
int	 AG_ObjectSaveToFile(void *, const char *);
#define	 AG_ObjectSave(p) AG_ObjectSaveToFile((p),NULL)
int	 AG_ObjectSaveAll(void *);

#define	 AG_ObjectLoad(p)         AG_ObjectLoadFromFile((p),NULL)
#define	 AG_ObjectLoadData(o,f)   AG_ObjectLoadDataFromFile((o),(f),NULL)
#define	 AG_ObjectLoadGeneric(p)  AG_ObjectLoadGenericFromFile((p),NULL)

int	 AG_ObjectLoadFromFile(void *, const char *);
int	 AG_ObjectLoadGenericFromFile(void *, const char *);

int	 AG_ObjectResolveDeps(void *);
int	 AG_ObjectLoadDataFromFile(void *, int *, const char *);

AG_ObjectDep	*AG_ObjectAddDep(void *, void *);
int	 	 AG_ObjectFindDep(const void *, Uint32, void **);
void		 AG_ObjectDelDep(void *, const void *);
Uint32		 AG_ObjectEncodeName(const void *, const void *);
void		*AG_ObjectEdit(void *);
void		 AG_ObjectGenName(AG_Object *, AG_ObjectClass *, char *,
		                  size_t);

/* Lock/unlock the property table of an object. */
static __inline__ void
AG_LockProps(void *p)
{
	AG_MutexLock(&AGOBJECT(p)->lock);
}
static __inline__ void
AG_UnlockProps(void *p)
{
	AG_MutexUnlock(&AGOBJECT(p)->lock);
}

/* Check if an object's class name matches the given pattern. */
static __inline__ int
AG_ObjectIsClass(const void *p, const char *cname)
{
	const AG_Object *obj = AGOBJECT(p);
	const char *c;
	int nwild = 0;

	if (cname[0] == '*' && cname[1] == '\0') {
		return (1);
	}
	for (c = &cname[0]; *c != '\0'; c++) {
		if (*c == '*')
			nwild++;
	}
	/* Optimize for simplest case (no wildcards). */
	if (nwild == 0) {
		return (strncmp(obj->cls->name, cname, c - &cname[0]) == 0);
	}
	/* Optimize for single-wildcard cases. */
	if (nwild == 1) {
		for (c = &cname[0]; *c != '\0'; c++) {
			if (c[0] == ':' && c[1] == '*' && c[2] == '\0') {
				if (c == &cname[0] ||
				    strncmp(obj->cls->name, cname,
				            c - &cname[0]) == 0)
					return (1);
			}
		}
		/* TODO: Optimize for "*:Foo" case */
	}
	/* Fallback to the general matching algorithm. */
	return (AG_ObjectIsClassGeneral(obj, cname));
}

/*
 * Return the root of a given object's ancestry.
 * The linkage must be locked.
 */
static __inline__ void *
AG_ObjectRoot(const void *p)
{
	const AG_Object *ob = AGOBJECT(p);

	while (ob != NULL) {
		if (ob->parent == NULL) {
			return ((void *)ob);
		}
		ob = AGOBJECT(ob->parent);
	}
	return (NULL);
}

/*
 * Traverse an object's ancestry looking for a matching parent object.
 * The linkage must be locked.
 */
static __inline__ void *
AG_ObjectFindParent(void *obj, const char *name, const char *t)
{
	AG_Object *ob = AGOBJECT(obj);

	while (ob != NULL) {
		AG_Object *po = AGOBJECT(ob->parent);

		if (po == NULL) {
			return (NULL);
		}
		if ((t == NULL || strcmp(po->cls->name, t) == 0) &&
		    (name == NULL || strcmp(po->name, name) == 0)) {
			return ((void *)po);
		}
		ob = AGOBJECT(ob->parent);
	}
	return (NULL);
}

/*
 * Return a child object by name.
 * The linkage must be locked.
 */
static __inline__ void *
AG_ObjectFindChild(void *p, const char *name)
{
	AG_Object *pObj = AGOBJECT(p);
	AG_Object *cObj;

	AGOBJECT_FOREACH_CHILD(cObj, pObj, ag_object) {
		if (strcmp(cObj->name, name) == 0)
			break;
	}
	return (cObj);
}
__END_DECLS

#include "close_code.h"
#endif	/* _AGAR_OBJECT_H */
