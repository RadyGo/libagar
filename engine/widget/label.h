/*	$Csoft: label.h,v 1.14 2002/11/14 07:18:33 vedge Exp $	*/
/*	Public domain	*/

#define LABEL_MAX_LENGTH	2048
#define LABEL_MAX_POLLITEMS	32

enum label_type {
	LABEL_STATIC,
	LABEL_POLLED
};

struct label {
	struct widget	wid;
	enum label_type	type;
	struct {
		char		*caption;
		SDL_Surface	*surface;
		pthread_mutex_t	 lock;
	} text;
	struct {
		char		*fmt;
		void		*ptrs[LABEL_MAX_POLLITEMS];
		int		 nptrs;
		pthread_mutex_t	*lock;
	} poll;
	enum {
		LABEL_LEFT,
		LABEL_CENTER,
		LABEL_RIGHT
	} justify;
};

struct label	*label_new(struct region *, int, int, const char *, ...);
struct label	*label_polled_new(struct region *, int, int, pthread_mutex_t *,
		     const char *, ...);
void		 label_init(struct label *, enum label_type, const char *,
		     int, int);
void	 	 label_destroy(void *);
void		 label_draw(void *);
void		 label_printf(struct label *, const char *, ...);

