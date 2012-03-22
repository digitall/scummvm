#ifndef __XBOXMISSING_H__
#define __XBOXMISSING_H__

#ifdef _XBOX

#ifdef  __cplusplus
extern "C" {
#endif

char* getenv(const char* szKey);
int putenv(const char* szKey);
//char* getcwd(const char* szKey);
void XExit(void);
void backslash(char *s);

#endif

#ifdef  __cplusplus
}
#endif
#endif /* __XBOXMISSING_H__ */
