
# include "so_model.h"

static int fac(int x)
{
	int factorial = 1;
	for (int i = 2; i < x; ++i)
		factorial *= i;
	return factorial;
}

static GsVec eval_bezier(float t, const GsArray<GsVec>& ctrlpnts)
{
	int n = ctrlpnts.size() - 1;					// summation's n
	GsVec result(0.0f, 0.0f, 0.0f);					// resulting point
	float b_func = 0;								// to determine Bernstein polynomial part
	float bin_coeff;								// binomial coefficient

	for (int c = 0; c <= n; ++c)
	{
		bin_coeff = (float)fac(n) / (fac(n - c)*fac(c));
		b_func = (bin_coeff*pow(t, c))*(pow((1 - t), n - c));
		result += ctrlpnts[c] * b_func;
	}
	return result;
}

SoModel::SoModel()
 {
 }

SoModel::SoModel(double x, double y, double z)
{
	_numpoints = 0;
	_phong = false;
	frame_num = 0;
	//moveYby = 0.0f;
	//moveZby = 0.0f;
	//moveXby = 0.0f;
	moveOffset = 0.025f;
	forwardVec = GsVec(0, 0, -1)*moveOffset;
	
	// Create Bezier
	numsegs = 60;
	ctrlpnts.push() = GsVec(x, y, z);
	ctrlpnts.push() = GsVec(x-0.5, y, z-0.5);
	ctrlpnts.push() = GsVec(x, y, z-1);
	ctrlpnts.push() = GsVec(x+1, y, x-1);
	ctrlpnts.push() = GsVec(x, y, z);
	for (int i = 0; i <= numsegs; ++i)
		coord.push() = eval_bezier(i * 1.0f / numsegs, ctrlpnts);
}


void SoModel::init ()
 {
   // Load programs:
   _vshgou.load_and_compile ( GL_VERTEX_SHADER, "../mcol_gouraud.vert" );
   _fshgou.load_and_compile ( GL_FRAGMENT_SHADER, "../gouraud.frag" );
   _proggouraud.init_and_link ( _vshgou, _fshgou );

   _vshphong.load_and_compile ( GL_VERTEX_SHADER, "../mcol_phong.vert" );
   _fshphong.load_and_compile ( GL_FRAGMENT_SHADER, "../mcol_phong.frag" );
   _progphong.init_and_link ( _vshphong, _fshphong );

   // Define buffers needed:
   gen_vertex_arrays ( 1 ); // will use 1 vertex array
   gen_buffers ( 3 );       // will use 3 buffers

   _proggouraud.uniform_locations ( 9 ); // will send 9 variables
   _proggouraud.uniform_location ( 0, "vTransf" );
   _proggouraud.uniform_location ( 1, "vProj" );
   _proggouraud.uniform_location ( 2, "lPos" );
   _proggouraud.uniform_location ( 3, "la" );
   _proggouraud.uniform_location ( 4, "ld" );
   _proggouraud.uniform_location ( 5, "ls" );
   _proggouraud.uniform_location ( 6, "ka" );
   _proggouraud.uniform_location ( 7, "ks" );
   _proggouraud.uniform_location ( 8, "sh" );

   _progphong.uniform_locations ( 9 ); // will send 9 variables
   _progphong.uniform_location ( 0, "vTransf" );
   _progphong.uniform_location ( 1, "vProj" );
   _progphong.uniform_location ( 2, "lPos" );
   _progphong.uniform_location ( 3, "la" );
   _progphong.uniform_location ( 4, "ld" );
   _progphong.uniform_location ( 5, "ls" );
   _progphong.uniform_location ( 6, "ka" );
   _progphong.uniform_location ( 7, "ks" );
   _progphong.uniform_location ( 8, "sh" );
 }



void SoModel::build ( GsModel& m )
 {
   int i;
   GsColor c;
   P.size(0); C.size(0); N.size(0);

   /* There are multiple ways to organize data to send to OpenGL. 
      Here we send material information per vertex but we only send the diffuse color
      information per vertex. The other components are uniforms sent to affect the whole object.
      This is a solution that keeps this code simple and is ok for most objects.
   */

   // build arrays:
   for ( i=0; i<m.F.size(); i++ )
    { GsModel::Face& f = m.F[i];
      P.push()=m.V[f.a]; P.push()=m.V[f.b]; P.push()=m.V[f.c]; 

      if ( m.Fn.size()>0 && i<m.Fn.size() )
       { GsModel::Face& f = m.Fn[i];
         N.push()=m.N[f.a]; N.push()=m.N[f.b]; N.push()=m.N[f.c];
         //std::cout<<i<<": "<<N.top()<<"\n";
       }
      else if ( m.N.size()>0 && i<m.N.size() )
       { GsVec n=m.N[i]; N.push()=n; N.push()=n; N.push()=n; }
      else
       { GsVec n=m.face_normal(i); N.push()=n; N.push()=n; N.push()=n; }

      c = GsColor::gray;
      if ( m.Fm.size()>0 && i<m.Fm.size() ) 
       { int id=m.Fm[i]; 
         if (id<0) { if (C.size()>0)c=C.top(); } else c=m.M[id].diffuse;
         //std::cout<<i<<"/"<<id<<" "<<c<<"\n";
       }
      else if ( m.M.size()>0 && i<m.M.size() ) 
       { c = m.M[i].diffuse; }
      C.push()=c; C.push()=c; C.push()=c;
    }

   if ( m.M.size()>0 ) _mtl=m.M[0]; else _mtl.init();

   // send data to OpenGL buffers:
   glBindVertexArray ( va[0] );
   glEnableVertexAttribArray ( 0 );
   glEnableVertexAttribArray ( 1 );
   glEnableVertexAttribArray ( 2 );

   glBindBuffer ( GL_ARRAY_BUFFER, buf[0] );
   glBufferData ( GL_ARRAY_BUFFER, 3*sizeof(float)*P.size(), P.pt(), GL_STATIC_DRAW );
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );

   glBindBuffer ( GL_ARRAY_BUFFER, buf[1] );
   glBufferData ( GL_ARRAY_BUFFER, 3*sizeof(float)*N.size(), N.pt(), GL_STATIC_DRAW );
   glVertexAttribPointer ( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );

   glBindBuffer ( GL_ARRAY_BUFFER, buf[2] );
   glBufferData ( GL_ARRAY_BUFFER, 4*sizeof(gsbyte)*C.size(), C.pt(), GL_STATIC_DRAW );
   glVertexAttribPointer ( 2, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0 );

   glBindVertexArray(0); // break the existing vertex array object binding.

   std::cout<<"build ok.\n";

   // save size so that we can free our buffers and later draw the OpenGL arrays:
   _numpoints = P.size();

   // free non-needed memory:
   P.capacity(0); C.capacity(0); N.capacity(0);
 }

void SoModel::draw ( const GsMat& tr, const GsMat& pr, const GsLight& l )
 {
   float f[4]; 
   float sh = (float)_mtl.shininess;
   if ( sh<0.001f ) sh=64;

   GsMat move;
   move.translation(coord[frame_num]);
   GsMat finalM = tr * move;

   if ( _phong )
    { glUseProgram ( _progphong.id );
      glUniformMatrix4fv ( _progphong.uniloc[0], 1, GL_FALSE, finalM.e );
      glUniformMatrix4fv ( _progphong.uniloc[1], 1, GL_FALSE, pr.e );
      glUniform3fv ( _progphong.uniloc[2], 1, l.pos.e );
      glUniform4fv ( _progphong.uniloc[3], 1, l.amb.get(f) );
      glUniform4fv ( _progphong.uniloc[4], 1, l.dif.get(f) );
      glUniform4fv ( _progphong.uniloc[5], 1, l.spe.get(f) );
      glUniform4fv ( _progphong.uniloc[6], 1, _mtl.ambient.get(f) );
      glUniform4fv ( _progphong.uniloc[7], 1, _mtl.specular.get(f) );
      glUniform1fv ( _progphong.uniloc[8], 1, &sh );
    }
   else
    { glUseProgram ( _proggouraud.id );
      glUniformMatrix4fv ( _proggouraud.uniloc[0], 1, GL_FALSE, finalM.e );
      glUniformMatrix4fv ( _proggouraud.uniloc[1], 1, GL_FALSE, pr.e );
      glUniform3fv ( _proggouraud.uniloc[2], 1, l.pos.e );
      glUniform4fv ( _proggouraud.uniloc[3], 1, l.amb.get(f) );
      glUniform4fv ( _proggouraud.uniloc[4], 1, l.dif.get(f) );
      glUniform4fv ( _proggouraud.uniloc[5], 1, l.spe.get(f) );
      glUniform4fv ( _proggouraud.uniloc[6], 1, _mtl.ambient.get(f) );
      glUniform4fv ( _proggouraud.uniloc[7], 1, _mtl.specular.get(f) );
      glUniform1fv ( _proggouraud.uniloc[8], 1, &sh );
    }

   glBindVertexArray ( va[0] );
   glDrawArrays ( GL_TRIANGLES, 0, _numpoints );
   glBindVertexArray(0); // break the existing vertex array object binding.
 }

void SoModel::move()
{
	if (frame_num == numsegs + 1)
		frame_num = 0;
	if (animate == true)
	{
		//std::cout << frame_num << "\n";
		//moveXby = coord[frame_num].x;
		//moveZby = coord[frame_num].z;
		frame_num++;
	}
}

