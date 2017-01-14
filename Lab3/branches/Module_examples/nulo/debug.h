#ifndef DEBUG_H
#define DEBUG_H

/**
 * Atención: la macro dprintk es altamente gcc-dependiente.
 */

#ifdef DEBUG
#define dprintk(format, ...)	printk(KERN_DEBUG format, ##__VA_ARGS__)
#else /* ! DEBUG */
#define dprintk(format, ...)
#endif /* DEBUG */

#endif /* DEBUG_H */

