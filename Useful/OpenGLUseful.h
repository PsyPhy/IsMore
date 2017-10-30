#define glTranslatedv( v )  glTranslated( v[X], v[Y], v[Z] )
#define glRotatedv( angle, v )  glRotated( angle, v[X], v[Y], v[Z] )

#ifdef  __cplusplus
  extern "C"
    {
#endif

void glUsefulDefaultSpecularLighting( double intensity ); // Directional
void glUsefulCodaLighting( void ); 
void glUsefulAutoLighting( double intensity );    // Non-Directional
void glUsefulDefaultMaterial( void );
void glUsefulShinyMaterial( void );
void glUsefulMatteMaterial( void );
// Initialize gl to a default state. TRUE for specular, FALSE for ambient only.
void glUsefulInitializeDefault( void );
// Prepare the gl state for a new rendering pass.
void glUsefulPrepareRendering ( void );


#ifdef  __cplusplus
  }
#endif
