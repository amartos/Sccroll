/**
 * @file        helpers.c
 * @version     0.1.0
 * @brief       Fichier source pour les aides à la programmation.
 * @date        2022
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   MIT License
 * @compilation @ref sccroll.h
 *
 * @addtogroup Internals
 * @{
 * @addtogroup Helpers Aides à la programmation.
 * @{
 */

#include "sccroll/helpers.h"

// clang-format off

/******************************************************************************
 * Implémentation
 ******************************************************************************/
// clang-format on

unsigned sccroll_hasFlags(unsigned flags, unsigned values)
{
    return (flags & values);
}

int sccroll_simplefork(const char* restrict desc, SccrollFunc callback)
{
    int status = 0;
    pid_t pid = fork();
    if (pid < 0) err(EXIT_FAILURE, "%s", desc);
    else if (pid == 0) callback(), exit(EXIT_SUCCESS);
    wait(&status);
    return status;
}

const char* strerrorname_np(int errnum)
{
    switch(errnum)
    {
    default: return NULL;
    case 0: return "0";
    case E2BIG: return "E2BIG";
    case EACCES: return "EACCES";
    case EADDRINUSE: return "EADDRINUSE";
    case EADDRNOTAVAIL: return "EADDRNOTAVAIL";
    case EADV: return "EADV";
    case EAFNOSUPPORT: return "EAFNOSUPPORT";
    case EAGAIN: return "EAGAIN";
#if defined EWOULDBLOCK && EAGAIN != EWOULDBLOCK
    case EWOULDBLOCK: return "EWOULDBLOCK";
#endif
    case EALREADY: return "EALREADY";
#ifdef EAUTH
    case EAUTH: return "EAUTH";
#endif
#ifdef EBACKGROUND
    case EBACKGROUND: return "EBACKGROUND";
#endif
    case EBADE: return "EBADE";
    case EBADF: return "EBADF";
    case EBADFD: return "EBADFD";
    case EBADMSG: return "EBADMSG";
    case EBADR: return "EBADR";
#ifdef EBADRPC
    case EBADRPC: return "EBADRPC";
#endif
    case EBADRQC: return "EBADRQC";
    case EBADSLT: return "EBADSLT";
    case EBFONT: return "EBFONT";
    case EBUSY: return "EBUSY";
    case ECANCELED: return "ECANCELED";
    case ECHILD: return "ECHILD";
    case ECHRNG: return "ECHRNG";
    case ECOMM: return "ECOMM";
    case ECONNABORTED: return "ECONNABORTED";
    case ECONNREFUSED: return "ECONNREFUSED";
    case ECONNRESET: return "ECONNRESET";
#ifdef ED
    case ED: return "ED";
#endif
    case EDEADLK: return "EDEADLK";
#if defined EDEADLOCK && EDEADLK != EDEADLOCK
    case EDEADLOCK: return "EDEADLOCK";
#endif
    case EDESTADDRREQ: return "EDESTADDRREQ";
#ifdef EDIED
    case EDIED: return "EDIED";
#endif
    case EDOM: return "EDOM";
    case EDOTDOT: return "EDOTDOT";
    case EDQUOT: return "EDQUOT";
    case EEXIST: return "EEXIST";
    case EFAULT: return "EFAULT";
    case EFBIG: return "EFBIG";
#ifdef EFTYPE
    case EFTYPE: return "EFTYPE";
#endif
#ifdef EGRATUITOUS
    case EGRATUITOUS: return "EGRATUITOUS";
#endif
#ifdef EGREGIOUS
    case EGREGIOUS: return "EGREGIOUS";
#endif
    case EHOSTDOWN: return "EHOSTDOWN";
    case EHOSTUNREACH: return "EHOSTUNREACH";
    case EHWPOISON: return "EHWPOISON";
    case EIDRM: return "EIDRM";
#ifdef EIEIO
    case EIEIO: return "EIEIO";
#endif
    case EILSEQ: return "EILSEQ";
    case EINPROGRESS: return "EINPROGRESS";
    case EINTR: return "EINTR";
    case EINVAL: return "EINVAL";
    case EIO: return "EIO";
    case EISCONN: return "EISCONN";
    case EISDIR: return "EISDIR";
    case EISNAM: return "EISNAM";
    case EKEYEXPIRED: return "EKEYEXPIRED";
    case EKEYREJECTED: return "EKEYREJECTED";
    case EKEYREVOKED: return "EKEYREVOKED";
    case EL2HLT: return "EL2HLT";
    case EL2NSYNC: return "EL2NSYNC";
    case EL3HLT: return "EL3HLT";
    case EL3RST: return "EL3RST";
    case ELIBACC: return "ELIBACC";
    case ELIBBAD: return "ELIBBAD";
    case ELIBEXEC: return "ELIBEXEC";
    case ELIBMAX: return "ELIBMAX";
    case ELIBSCN: return "ELIBSCN";
    case ELNRNG: return "ELNRNG";
    case ELOOP: return "ELOOP";
    case EMEDIUMTYPE: return "EMEDIUMTYPE";
    case EMFILE: return "EMFILE";
    case EMLINK: return "EMLINK";
    case EMSGSIZE: return "EMSGSIZE";
    case EMULTIHOP: return "EMULTIHOP";
    case ENAMETOOLONG: return "ENAMETOOLONG";
    case ENAVAIL: return "ENAVAIL";
#ifdef ENEEDAUTH
    case ENEEDAUTH: return "ENEEDAUTH";
#endif
    case ENETDOWN: return "ENETDOWN";
    case ENETRESET: return "ENETRESET";
    case ENETUNREACH: return "ENETUNREACH";
    case ENFILE: return "ENFILE";
    case ENOANO: return "ENOANO";
    case ENOBUFS: return "ENOBUFS";
    case ENOCSI: return "ENOCSI";
    case ENODATA: return "ENODATA";
    case ENODEV: return "ENODEV";
    case ENOENT: return "ENOENT";
    case ENOEXEC: return "ENOEXEC";
    case ENOKEY: return "ENOKEY";
    case ENOLCK: return "ENOLCK";
    case ENOLINK: return "ENOLINK";
    case ENOMEDIUM: return "ENOMEDIUM";
    case ENOMEM: return "ENOMEM";
    case ENOMSG: return "ENOMSG";
    case ENONET: return "ENONET";
    case ENOPKG: return "ENOPKG";
    case ENOPROTOOPT: return "ENOPROTOOPT";
    case ENOSPC: return "ENOSPC";
    case ENOSR: return "ENOSR";
    case ENOSTR: return "ENOSTR";
    case ENOSYS: return "ENOSYS";
    case ENOTBLK: return "ENOTBLK";
    case ENOTCONN: return "ENOTCONN";
    case ENOTDIR: return "ENOTDIR";
    case ENOTEMPTY: return "ENOTEMPTY";
    case ENOTNAM: return "ENOTNAM";
    case ENOTRECOVERABLE: return "ENOTRECOVERABLE";
    case ENOTSOCK: return "ENOTSOCK";
    case ENOTTY: return "ENOTTY";
    case ENOTUNIQ: return "ENOTUNIQ";
    case ENXIO: return "ENXIO";
    case EOPNOTSUPP: return "EOPNOTSUPP";
#if defined ENOTSUPP && ENOTSUP != EOPNOTSUPP
    case ENOTSUP: return "ENOTSUP";
#endif
    case EOVERFLOW: return "EOVERFLOW";
    case EOWNERDEAD: return "EOWNERDEAD";
    case EPERM: return "EPERM";
    case EPFNOSUPPORT: return "EPFNOSUPPORT";
    case EPIPE: return "EPIPE";
#ifdef EPROCLIM
    case EPROCLIM: return "EPROCLIM";
#endif
#ifdef EPROCUNAVAIL
    case EPROCUNAVAIL:  return "EPROCUNAVAIL";
#endif
#ifdef EPROGMISMATCH
    case EPROGMISMATCH:  return "EPROGMISMATCH";
#endif
#ifdef EPROGUNAVAIL
    case EPROGUNAVAIL:  return "EPROGUNAVAIL";
#endif
    case EPROTO: return "EPROTO";
    case EPROTONOSUPPORT: return "EPROTONOSUPPORT";
    case EPROTOTYPE: return "EPROTOTYPE";
    case ERANGE: return "ERANGE";
    case EREMCHG: return "EREMCHG";
    case EREMOTE: return "EREMOTE";
    case EREMOTEIO: return "EREMOTEIO";
    case ERESTART: return "ERESTART";
    case ERFKILL: return "ERFKILL";
    case EROFS: return "EROFS";
#ifdef ERPCMISMATCH
    case ERPCMISMATCH:  return "ERPCMISMATCH";
#endif
    case ESHUTDOWN: return "ESHUTDOWN";
    case ESOCKTNOSUPPORT: return "ESOCKTNOSUPPORT";
    case ESPIPE: return "ESPIPE";
    case ESRCH: return "ESRCH";
    case ESRMNT: return "ESRMNT";
    case ESTALE: return "ESTALE";
    case ESTRPIPE: return "ESTRPIPE";
    case ETIME: return "ETIME";
    case ETIMEDOUT: return "ETIMEDOUT";
    case ETOOMANYREFS: return "ETOOMANYREFS";
    case ETXTBSY: return "ETXTBSY";
    case EUCLEAN: return "EUCLEAN";
    case EUNATCH: return "EUNATCH";
    case EUSERS: return "EUSERS";
    case EXDEV: return "EXDEV";
    case EXFULL: return "EXFULL";
    }
}

const char* sigabbrev_np(int sig)
{
    switch(sig)
    {
    default: return NULL;
    case SIGABRT: return "ABRT";
#if defined SIGIOT && SIGIOT != SIGABRT
    case SIGIOT: return "IOT";
#endif
    case SIGALRM: return "ALRM";
    case SIGBUS: return "BUS";
    case SIGCHLD: return "CHLD";
#if defined SIGCLD && SIGCLD != SIGCHLD
    case SIGCLD: return "CLD";
#endif
    case SIGCONT: return "CONT";
#ifdef SIGEMT
    case SIGEMT: return "EMT";
#endif
    case SIGFPE: return "FPE";
    case SIGHUP: return "HUP";
    case SIGILL: return "ILL";
    case SIGINT: return "INT";
    case SIGIO: return "IO";
#if defined SIGPOLL && SIGPOLL != SIGIO
    case SIGPOLL: return "POLL";
#endif
    case SIGKILL: return "KILL";
#ifdef SIGLOST
    case SIGLOST: return "LOST";
#endif
    case SIGPIPE: return "PIPE";
    case SIGPROF: return "PROF";
    case SIGPWR: return "PWR";
#if defined SIGINFO && SIGINFO != SIGPWR
    case SIGINFO: return "INFO";
#endif
    case SIGQUIT: return "QUIT";
    case SIGSEGV: return "SEGV";
    case SIGSTKFLT: return "STKFLT";
    case SIGSTOP: return "STOP";
    case SIGSYS: return "SYS";
#if defined SIGUNUSED && SIGUNUSED != SIGSYS
    case SIGUNUSED: return "UNUSED";
#endif
    case SIGTERM: return "TERM";
    case SIGTRAP: return "TRAP";
    case SIGTSTP: return "TSTP";
    case SIGTTIN: return "TTIN";
    case SIGTTOU: return "TTOU";
    case SIGURG: return "URG";
    case SIGUSR1: return "USR1";
    case SIGUSR2: return "USR2";
    case SIGVTALRM: return "VTALRM";
    case SIGWINCH: return "WINCH";
    case SIGXCPU: return "XCPU";
    case SIGXFSZ: return "XFSZ";
    }
}

/** @} @} */
