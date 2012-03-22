#ifndef __XBOXCONFIG_H__
#define __XBOXCONFIG_H__

#ifdef _XBOX

#ifdef  __cplusplus
extern "C" {
#endif

void XboxSetScreen (void);
void XboxLoadConfig (void);
void XboxSaveConfig (void);

#endif

#ifdef  __cplusplus
}
#endif
#endif /* __XBOXCONFIG_H__ */
