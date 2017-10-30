/*********************************************************************************/
/*                                                                               */
/*                                   VectorsMixin.cpp                            */
/*                                                                               */
/*********************************************************************************/

// Suport routines for vector and matrix operations.
// Vectors here are 3D. Quaternions are 4D.

// This is the main DLL file.

#include "stdafx.h"
#include "../Useful/Useful.h" 
#include "../Useful/fOutputDebugString.h" 
#include "../Useful/fMessageBox.h"

#include "VectorsMixin.h"

using namespace PsyPhy;

const double VectorsMixin::pi = 3.14159265358979;
const double VectorsMixin::epsilon = 0.0001;

const Vector3 VectorsMixin::zeroVector = {0.0, 0.0, 0.0};
const Vector3 VectorsMixin::iVector = { 1.0, 0.0, 0.0 };
const Vector3 VectorsMixin::jVector = { 0.0, 1.0, 0.0 };
const Vector3 VectorsMixin::kVector = { 0.0, 0.0, 1.0 };

const Vector3 VectorsMixin::iVectorMinus = { -1.0, 0.0, 0.0 };
const Vector3 VectorsMixin::jVectorMinus = { 0.0, -1.0, 0.0 };
const Vector3 VectorsMixin::kVectorMinus = { 0.0, 0.0, -1.0 };

const Quaternion VectorsMixin::nullQuaternion = {0.0, 0.0, 0.0, 1.0};

const Pose VectorsMixin::nullPose = {{ 0.0, 0.0, 0.0 }, {0.0, 0.0, 0.0, 1.0}};

const Matrix3x3 VectorsMixin::identityMatrix = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
const Matrix3x3 VectorsMixin::zeroMatrix =     {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

double VectorsMixin::ToDegrees( double radians ) { return( radians * 180.0 / pi ); }
double VectorsMixin::ToRadians( double degrees ) { return( degrees * pi / 180.0 ); }


void VectorsMixin::CopyVector( Vector3 destination, const Vector3 source ){
	destination[X] = source[X];
	destination[Y] = source[Y];
	destination[Z] = source[Z];
}
void VectorsMixin::CopyVector( Vector3f destination, const Vector3 source ){
	destination[X] = (float) source[X];
	destination[Y] = (float) source[Y];
	destination[Z] = (float) source[Z];
}
void VectorsMixin::CopyVector( Vector3 destination, const Vector3f source ){
	destination[X] = (double) source[X];
	destination[Y] = (double) source[Y];
	destination[Z] = (double) source[Z];
}
void VectorsMixin::CopyVector( Vector3f destination, const Vector3f source ){
	destination[X] = source[X];
	destination[Y] = source[Y];
	destination[Z] = source[Z];
}

void VectorsMixin::CopyQuaternion( Quaternion destination, const Quaternion source ){
	destination[X] = source[X];
	destination[Y] = source[Y];
	destination[Z] = source[Z];
	destination[M] = source[M];
}
void VectorsMixin::CopyQuaternion( fQuaternion destination, const Quaternion source ){
	destination[X] = (float) source[X];
	destination[Y] = (float) source[Y];
	destination[Z] = (float) source[Z];
	destination[M] = (float) source[M];
}
void VectorsMixin::CopyQuaternion( Quaternion destination, const fQuaternion source ){
	destination[X] = (double) source[X];
	destination[Y] = (double) source[Y];
	destination[Z] = (double) source[Z];
	destination[M] = (double) source[M];
}
void VectorsMixin::CopyQuaternion( fQuaternion destination, const fQuaternion source ){
	destination[X] = source[X];
	destination[Y] = source[Y];
	destination[Z] = source[Z];
	destination[M] = source[M];
}

void VectorsMixin::CopyPose( Pose &destination, const Pose &source ) {
	CopyVector( destination.position, source.position );
	CopyQuaternion( destination.orientation, source.orientation );
}

void VectorsMixin::CopyPose( CompactPose &destination, const Pose &source ) {

	destination.position[X] = (short) (source.position[X] * 10.0);
	destination.position[Y] = (short) (source.position[Y] * 10.0);
	destination.position[Z] = (short) (source.position[Z] * 10.0);

	destination.orientation[X] = (float) source.orientation[X];
	destination.orientation[Y] = (float) source.orientation[Y];
	destination.orientation[Z] = (float) source.orientation[Z];
	destination.orientation[M] = (float) source.orientation[M];

}

void VectorsMixin::CopyPose( Pose &destination, const CompactPose &source ) {

	destination.position[X] = (short) (source.position[X] / 10.0);
	destination.position[Y] = (short) (source.position[Y] / 10.0);
	destination.position[Z] = (short) (source.position[Z] / 10.0);

	destination.orientation[X] = (double) source.orientation[X];
	destination.orientation[Y] = (double) source.orientation[Y];
	destination.orientation[Z] = (double) source.orientation[Z];
	destination.orientation[M] = (double) source.orientation[M];

}
void VectorsMixin::AddVectors( Vector3 result, const Vector3f a, const Vector3f b ) {
	result[X] = (double) ( a[X] + b[X] );
	result[Y] = (double) ( a[Y] + b[Y] );
	result[Z] = (double) ( a[Z] + b[Z] );
} 

void VectorsMixin::AddVectors( Vector3 result, const Vector3 a, const Vector3 b ) {
	result[X] = a[X] + b[X];
	result[Y] = a[Y] + b[Y];
	result[Z] = a[Z] + b[Z];
}

void VectorsMixin::AddVectors( Vector3 result, const Vector3f a, const Vector3 b ) {
	result[X] = (double) a[X] + b[X];
	result[Y] = (double) a[Y] + b[Y];
	result[Z] = (double) a[Z] + b[Z];
}

void VectorsMixin::AddVectors( Vector3 result, const Vector3 a, const Vector3f b ) {
	result[X] = a[X] + (double) b[X];
	result[Y] = a[Y] + (double) b[Y];
	result[Z] = a[Z] + (double) b[Z];
}

void VectorsMixin::AddVectors( Vector3f result, const Vector3 a, const Vector3 b ) {
	result[X] = (float) ( a[X] + b[X] );
	result[Y] = (float) ( a[Y] + b[Y] );
	result[Z] = (float) ( a[Z] + b[Z] );
} 

void VectorsMixin::AddVectors( Vector3f result, const Vector3f a, const Vector3f b ) {
	result[X] = a[X] + b[X];
	result[Y] = a[Y] + b[Y];
	result[Z] = a[Z] + b[Z];
}

void VectorsMixin::SubtractVectors( Vector3 result, const Vector3 a, const Vector3 b ) {
	result[X] = a[X] - b[X];
	result[Y] = a[Y] - b[Y];
	result[Z] = a[Z] - b[Z];
}

void VectorsMixin::SubtractVectors( Vector3 result, const Vector3f a, const Vector3f b ) {
	result[X] = (double) ( a[X] - b[X] );
	result[Y] = (double) ( a[Y] - b[Y] );
	result[Z] = (double) ( a[Z] - b[Z] );
}

void VectorsMixin::SubtractVectors( Vector3 result, const Vector3f a, const Vector3 b ) {
	result[X] = (double) a[X] - b[X];
	result[Y] = (double) a[Y] - b[Y];
	result[Z] = (double) a[Z] - b[Z];
}

void VectorsMixin::SubtractVectors( Vector3f result, const Vector3 a, const Vector3 b ) {
	result[X] = (float) ( a[X] - b[X] );
	result[Y] = (float) ( a[Y] - b[Y] );
	result[Z] = (float) ( a[Z] - b[Z] );
}

void VectorsMixin::SubtractVectors( Vector3f result, const Vector3f a, const Vector3f b ) {
	result[X] = a[X] - b[X];
	result[Y] = a[Y] - b[Y];
	result[Z] = a[Z] - b[Z];
}



void VectorsMixin::ScaleVector( Vector3 result, const Vector3 a, const double scaling ) {
	result[X] = (float) a[X] * scaling;
	result[Y] = (float) a[Y] * scaling;
	result[Z] = (float) a[Z] * scaling;
}

void VectorsMixin::ScaleVector( Vector3f result, const Vector3 a, const double scaling ) {
	result[X] = (float) ( a[X] * scaling );
	result[Y] = (float) ( a[Y] * scaling );
	result[Z] = (float) ( a[Z] * scaling );
} 

void VectorsMixin::ScaleVector( Vector3f result, const Vector3f a, const double scaling ) {
	result[X] = (float) ( a[X] * scaling );
	result[Y] = (float) ( a[Y] * scaling );
	result[Z] = (float) ( a[Z] * scaling );
} 


double VectorsMixin::VectorNorm( const Vector3 vector ) {
	return( sqrt( vector[X] * vector[X] + vector[Y] * vector[Y] + vector[Z] * vector[Z] ) );
}

void VectorsMixin::NormalizeVector( Vector3 v ) {
	// Normalize a vector in place.
	ScaleVector( v, v, 1.0 / VectorNorm(v) );
}

double VectorsMixin::DotProduct( const Vector3 v1, const Vector3 v2 ) {
	return( v1[X] * v2[X] + v1[Y] * v2[Y] + v1[Z] * v2[Z] );
}

double VectorsMixin::AngleBetweenVectors( const Vector3 v1, const Vector3 v2 ) {
	return( acos( DotProduct( v1, v2 ) / VectorNorm( v1 ) / VectorNorm( v2 )) );
}

void VectorsMixin::ComputeCrossProduct( Vector3 result, const Vector3 v1, const Vector3 v2 ) {
	result[X] = v1[Y] * v2[Z] - v1[Z] * v2[Y];
	result[Y] = v1[Z] * v2[X] - v1[X] * v2[Z];
	result[Z] = v1[X] * v2[Y] - v1[Y] * v2[X];
}

void VectorsMixin::CopyMatrix( Matrix3x3 destination, const Matrix3x3 source ){
	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			destination[i][j] = source[i][j];
		}
	}
}
void VectorsMixin::CopyMatrix( fMatrix3x3 destination, const Matrix3x3 source ){
	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			destination[i][j] = (float) source[i][j];
		}
	}
}
void VectorsMixin::CopyMatrix( Matrix3x3 destination, const fMatrix3x3 source ){
	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			destination[i][j] = (double) source[i][j];
		}
	}
}
void VectorsMixin::CopyMatrix( fMatrix3x3 destination, const fMatrix3x3 source ){
	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			destination[i][j] = source[i][j];
		}
	}
}

void VectorsMixin::TransposeMatrix( Matrix3x3 destination, const Matrix3x3 source ){
	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			destination[j][i] = source[i][j];
		}
	}
}

void VectorsMixin::ScaleMatrix( Matrix3x3 destination, const Matrix3x3 source, const double scaling ){
	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			destination[i][j] = scaling * source[i][j];
		}
	}
}
void VectorsMixin::MultiplyMatrices( Matrix3x3 result, const Matrix3x3 left, const Matrix3x3 right ) {
	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			result[i][j] = 0.0;
			for ( int k = 0; k < 3; k++ ) result[i][j] += left[i][k] * right[k][j];
		}
	}
}

void VectorsMixin::OrthonormalizeMatrix( Matrix3x3 result, Matrix3x3 m ) {

	// Make sure that a matrix is orthonormal.
	// Take the first direction vector as the X axis.
	CopyVector( result[X], m[X] );
	// The second vector is assumed to lie in the XY plane.
	// Use it to compute the Z direction vector.
	ComputeCrossProduct( result[Z], m[X], m[Y] );
	// Now recompute the Y direction vector sure to be normal to X and Z.
	ComputeCrossProduct( result[Y], result[Z], m[X] );

	// All the direction vectors should be of unit length.
	NormalizeVector( result[X] );
	NormalizeVector( result[Y] );
	NormalizeVector( result[Z] );

}

void VectorsMixin::MultiplyVector( Vector3 result, const Vector3 v, const Matrix3x3 m ) {
	// I represent vectors as row vectors so that a matrix is an array of rows.
	// Therefore we normally do right multiplies.
	for ( int i = 0; i < 3; i++ ) {
		result[i] = 0.0;
		for ( int j = 0; j < 3; j++ ) result[i] += v[j] * m[j][i];
	}
}

void VectorsMixin::MultiplyVector( Vector3 result, const Vector3f v, const Matrix3x3 m ) {
	// I represent vectors as row vectors so that a matrix is an array of rows.
	// Therefore we normally do right multiplies.
	for ( int i = 0; i < 3; i++ ) {
		result[i] = 0.0;
		for ( int j = 0; j < 3; j++ ) result[i] += (double) v[j] * m[j][i];
	}
}

void VectorsMixin::MultiplyVector( Vector3f result, const Vector3f v, const Matrix3x3 m ) {
	// I represent vectors as row vectors so that a matrix is an array of rows.
	// Therefore we normally do right multiplies.
	for ( int i = 0; i < 3; i++ ) {
		result[i] = 0.0;
		for ( int j = 0; j < 3; j++ ) result[i] += (float) ((double) v[j] * m[j][i]);
	}
}

void VectorsMixin::MultiplyVector( Vector3f result, const Vector3 v, const Matrix3x3 m ) {
	// I represent vectors as row vectors so that a matrix is an array of rows.
	// Therefore we normally do right multiplies.
	for ( int i = 0; i < 3; i++ ) {
		result[i] = 0.0;
		for ( int j = 0; j < 3; j++ ) result[i] += (float) (v[j] * m[j][i]);
	}
}



// Let left and right be matrices of N 3-element row vectors.
// Compute transpose(left) * right, which is necessarily a 3x3 matrix
void VectorsMixin::CrossVectors( Matrix3x3 result, const Vector3 left[], const Vector3 right[], int rows ) {
	// Use doubles to compute the sums, to guard againts underflow.
	double r[3][3];
	int i, j, k;
	for ( i = 0; i < 3; i++ ) {
		for ( j = 0; j < 3; j++ ) {
			r[i][j] = 0.0;
			for ( k = 0; k < rows; k++ ) r[i][j] += left[k][i] * right[k][j];
		}
	}
	for ( i = 0; i < 3; i++ ) {
		for ( j = 0; j < 3; j++ ) {
			result[i][j] = r[i][j] / (double) rows;
		}
	}
}

void VectorsMixin::BestFitTransformation( Matrix3x3 result, const Vector3 input[], const Vector3 output[], int rows ) {

	Matrix3x3	left;
	Matrix3x3	right;
	Matrix3x3	left_inverse;

	// This computes the Moore-Penrose pseudo-inverse.
	CrossVectors( right, input, output, rows );
	CrossVectors( left, input, input, rows );
	InvertMatrix( left_inverse, left );
	MultiplyMatrices( result, left_inverse, right );

}

double VectorsMixin::Determinant( const Matrix3x3 m ) {

	return( 
		m[0][0] * ( m[2][2] * m[1][1] - m[2][1] * m[1][2] ) -
		m[1][0] * ( m[2][2] * m[0][1] - m[2][1] * m[0][2] ) +
		m[2][0] * ( m[1][2] * m[0][1] - m[1][1] * m[0][2] ) 
	);

}

double VectorsMixin::InvertMatrix( Matrix3x3 result, const Matrix3x3 m ){

	Matrix3x3 r;
	double det = Determinant( m );

	r[0][0] = m[2][2] * m[1][1] - m[2][1] * m[1][2];
	r[1][0] = m[2][0] * m[1][2] - m[2][2] * m[1][0];
	r[2][0] = m[2][1] * m[1][0] - m[2][0] * m[1][1];

	r[0][1] = m[2][1] * m[0][2] - m[2][2] * m[0][1];
	r[1][1] = m[2][2] * m[0][0] - m[2][0] * m[0][2];
	r[2][1] = m[2][0] * m[0][1] - m[2][1] * m[0][0];

	r[0][2] = m[1][2] * m[0][1] - m[1][1] * m[0][2];
	r[1][2] = m[1][0] * m[0][2] - m[1][2] * m[0][0];
	r[2][2] = m[1][1] * m[0][0] - m[1][0] * m[0][1];

	ScaleMatrix( result, r, 1.0 / det );

	return( det );

}


/*************************************************************************************************/

void VectorsMixin::NormalizeQuaternion( Quaternion q ) {
	double norm = sqrt( q[M] * q[M] + q[X] * q[X] + q[Y] * q[Y] + q[Z] * q[Z] );
	q[M] /= norm;
	q[X] /= norm;
	q[Y] /= norm;
	q[Z] /= norm;
}

void VectorsMixin::MultiplyQuaternions( Quaternion result, const Quaternion q1, const Quaternion q2 ) {

	// This routine gives incorrect results if the destination of the result is the same as either
	// of the multiplicands. To keep things efficient, we impose that q1 and q2 must
	// be different from result. That way we do not do unecessary copies when not needed.
	// The caller will have to copy to an intermediate variable if need be.
	assert( q1 != result && q2 != result ) ;

	result[M] = q1[M] * q2[M] - q1[X] * q2[X] - q1[Y] * q2[Y] - q1[Z] * q2[Z];
	result[X] = q1[M] * q2[X] + q1[X] * q2[M] + q1[Y] * q2[Z] - q1[Z] * q2[Y];
	result[Y] = q1[M] * q2[Y] - q1[X] * q2[Z] + q1[Y] * q2[M] + q1[Z] * q2[X];
	result[Z] = q1[M] * q2[Z] + q1[X] * q2[Y] - q1[Y] * q2[X] + q1[Z] * q2[M];

}

void VectorsMixin::RotateVector( Vector3 result, const Quaternion q, const Vector3 v ) {

	Quaternion vq;
	Quaternion rq;
	Quaternion interim;
	Quaternion conjugate;

	// Careful. I'm abusing CopyVector() here.
	// I use it to copy the first 3 components and then I set the 4th.
	vq[M] = 0.0;  CopyVector( vq, v );
	// Now I am abusing ScaleVector to take the conjugate of q.
	conjugate[M] = q[M]; ScaleVector( conjugate, q, -1.0 );
	// To rotate a vector, compute qvq*.
	MultiplyQuaternions( interim, q, vq );
	MultiplyQuaternions( rq, interim, conjugate );
	// Take just the vector part for the result.
	CopyVector( result, rq );

}

void VectorsMixin::SetQuaternion( Quaternion result, double radians, const Vector3 axis ) {

	// Compute the quaternion, making sure that the specified axis is a unit vector.
	result[M] = cos( 0.5 * radians );
	ScaleVector( result, axis, sin( 0.5 * radians ) / VectorNorm( axis ) );

}

// Compute a rotation matrix that will rotat v1 to align with v2, ignoring the roll around the vectors.
void VectorsMixin::SetRotationMatrix( Matrix3x3 result, const Vector3 v2, const Vector3 v1 ) {
	// Compute the rotation vector as the cross product of the unitized vectors.
	Vector3 u1, u2;
	CopyVector( u1, v1 );
	NormalizeVector( u1 );
	CopyVector( u2, v2 );
	NormalizeVector( u2 );
	Vector3 rotation_vector;
	ComputeCrossProduct( rotation_vector, u1, u2 );
	// Compute the magnitude of the rotation in a way that is not sensitive to parallel 
	//  vectors or to numerical errors when the rotation is zero.
	double angle = atan2( VectorNorm( rotation_vector ), DotProduct( u1, u2 ) );
	// Compute the rotation matrix. Note that we use the non-unit rotation vector as the axis of 
	//  rotation because SetRotationMatrix() will normalize it anyway.
	SetRotationMatrix( result, angle, rotation_vector );
}


void VectorsMixin::SetRotationMatrix( Matrix3x3 result, double radians, const Vector3 axis ) {

	// This needs to be checked. Is the formula for row vectors or column vectors?
	Vector3 n;
	double c, s, v;

	if ( fabs( radians ) < epsilon ) CopyMatrix( result, identityMatrix );
	else {

		CopyVector( n, axis );
		NormalizeVector( n );

		c = cos( radians );
		s = sin( radians );
		v = ( 1 - c );

		result[X][X] = n[X] * n[X] * v + c;
		result[Y][X] = n[Y] * n[X] * v - n[Z] * s;
		result[Z][X] = n[Z] * n[X] * v + n[Y] * s;

		result[X][Y] = n[X] * n[Y] * v + n[Z] * s;
		result[Y][Y] = n[Y] * n[Y] * v + c;
		result[Z][Y] = n[Z] * n[Y] * v - n[X] * s;

		result[X][Z] = n[X] * n[Z] * v - n[Y] * s;
		result[Y][Z] = n[Y] * n[Z] * v + n[X] * s;
		result[Z][Z] = n[Z] * n[Z] * v + c;

	}

}

void VectorsMixin::SetRotationMatrix( Matrix3x3 m, double roll, double pitch, double yaw ) {

	Matrix3x3 yawM, pitchM, rollM;
	Matrix3x3 rpM;

	CopyMatrix( yawM, identityMatrix );
	yawM[X][X] = cos( yaw );
	yawM[X][Z] = - sin( yaw );
	yawM[Z][X] = sin( yaw );
	yawM[Z][Z] = cos( yaw );

	CopyMatrix( pitchM, identityMatrix );
	pitchM[Y][Y] = cos( pitch );
	pitchM[Y][Z] = - sin( pitch );
	pitchM[Z][Y] = sin( pitch );
	pitchM[Z][Z] = cos( pitch );

	CopyMatrix( rollM, identityMatrix );
	rollM[X][X] = cos( roll );
	rollM[X][Y] = - sin( roll );
	rollM[Y][X] = sin( roll );
	rollM[Y][Y] = cos( roll );

	MultiplyMatrices( rpM, rollM, pitchM );
	MultiplyMatrices( m, rpM, yawM );

}

double VectorsMixin::RollAngleFromMatrix( const Matrix3x3 m ) {
	Matrix3x3 rollM;
	fAbortMessage( "VectorsMixin", "RollAngleFromMatrix() does not work yet." );
#if 1
	double azimuth = atan2( m[Z][X],m[Z][Z] );
	double elevation= atan2( m[Z][Y], sqrt(  m[Z][X] * m[Z][X] +m[Z][Z] * m[Z][Z] ) );

	Matrix3x3 yawMt;
	CopyMatrix( yawMt, identityMatrix );
	yawMt[X][X] = cos( azimuth );
	yawMt[X][Z] = sin( azimuth );
	yawMt[Z][X] = - sin( azimuth );
	yawMt[Z][Z] = cos( azimuth );

	Matrix3x3 pitchMt;
	CopyMatrix( pitchMt, identityMatrix );
	pitchMt[Y][Y] = cos( elevation );
	pitchMt[Y][Z] = sin( elevation );
	pitchMt[Z][Y] = - sin( elevation );
	pitchMt[Z][Z] = cos( elevation );

	Matrix3x3 pitchMyawMt;
	MultiplyMatrices( pitchMyawMt, yawMt, pitchMt );
	MultiplyMatrices( rollM, m, pitchMyawMt );
	Matrix3x3 aim;
	SetRotationMatrix( aim, kVector, m[Z] );
	//printf( "\n%s\n", mstr( pitchMyawMt ) );
	//printf( "%s\n", mstr( aim ) );
	Matrix3x3 aimT;
	TransposeMatrix( aimT, aim );
	MultiplyMatrices( rollM, pitchMyawMt, aimT );
	//printf( "%s\n", mstr( CHK ) );


#else
	MultiplyMatrices( rollM, aim, m );
#endif
	return( atan2( rollM[Y][X], rollM[X][X] ) );
}



void VectorsMixin::QuaternionToMatrix( Matrix3x3 m, const Quaternion q ) {

	m[X][X] = 1 - 2.0 * q[Y] * q[Y] - 2.0 * q[Z] * q[Z];
	m[Y][Y] = 1 - 2.0 * q[X] * q[X] - 2.0 * q[Z] * q[Z];
	m[Z][Z] = 1 - 2.0 * q[X] * q[X] - 2.0 * q[Y] * q[Y];

	m[X][Y] = 2.0 * q[X] * q[Y] + 2.0 * q[Z] * q[M];
	m[Y][X] = 2.0 * q[X] * q[Y] - 2.0 * q[Z] * q[M];

	m[X][Z] = 2.0 * q[X] * q[Z] - 2.0 * q[Y] * q[M];
	m[Z][X] = 2.0 * q[X] * q[Z] + 2.0 * q[Y] * q[M];

	m[Y][Z] = 2.0 * q[Y] * q[Z] + 2.0 * q[X] * q[M];
	m[Z][Y] = 2.0 * q[Y] * q[Z] - 2.0 * q[X] * q[M];

}

void VectorsMixin::MatrixToQuaternion( Quaternion result, Matrix3x3 m ) {

	Matrix3x3 ortho;
	double t, r, s;

	// Extract the orthonormal (rotation) part of the matrix.
	// Using this method is perhaps not the best solution. 
	// But the best solution requires computing eigenvalues and eigenvectors.
	// So I think that using this method is OK.
	OrthonormalizeMatrix( ortho, m );

#ifdef DEX_IMPLEMENTATION

	// At the time that I defined the DEX routines, I thought that there would be no
	// problem with singularities and large rotations. So I used only this formula.
	// But if the setup is in the supine position and if one computes the orientation
	// of the target frame, this solution is ill-conditioned. If one want to 
	// simulate DEX behavior, then better define DEX_IMPLEMENTATION. Otherwise, the following
	// algorithm is more robust.

	// NB Both algorithms assume that the rotation matrix is set up for row vectors and right multiplication
	//    of a vector. So the formulas may appear to be backwards, since what you find on 
	//    the web is likely to be for left multiplication.

	t = ortho[X][X] + ortho[Y][Y] + ortho[Z][Z];
	r = sqrt( 1.0 + t );
	s = 0.5 / r;

	result[M] = 0.5 * r;
	result[X] = ( ortho[Y][Z] - ortho[Z][Y] ) * s;
	result[Y] = ( ortho[Z][X] - ortho[X][Z] ) * s;
	result[Z] = ( ortho[X][Y] - ortho[Y][X] ) * s;

#else

	// This implementation avoids ill-conditioned responses.

	// NB Both algorithms assume that the rotation matrix is set up for row vectors and right multiplication
	//    of a vector. So the subtractions may appear to be backwards, since what you find on 
	//    the web is likely to be for left multiplication.

	t = ortho[X][X] + ortho[Y][Y] + ortho[Z][Z];

	if ( t > 0.0 ) {
		r = sqrt( 1.0 + ortho[X][X] + ortho[Y][Y] + ortho[Z][Z] );
		s = 0.5 / r;
		result[M] = 0.5 * r;
		result[X] = ( ortho[Y][Z] - ortho[Z][Y] ) * s;
		result[Y] = ( ortho[Z][X] - ortho[X][Z] ) * s;
		result[Z] = ( ortho[X][Y] - ortho[Y][X] ) * s;
	}
	else if ( (ortho[X][X] > ortho[Y][Y]) && (ortho[X][X] > ortho[Z][Z]) ) {
		r = sqrt( 1.0 + ortho[X][X] - ortho[Y][Y] - ortho[Z][Z] );
		s = 0.5 / r;
		result[X] = 0.5 * r;
		result[M] = ( ortho[Y][Z] - ortho[Z][Y] ) * s;
		result[Y] = ( ortho[Y][X] + ortho[X][Y] ) * s;
		result[Z] = ( ortho[Z][X] + ortho[X][Z] ) * s;

	}
	else if ( ortho[Y][Y] > ortho[Z][Z] ) {
		r = sqrt( 1.0 + ortho[Y][Y] - ortho[X][X] - ortho[Z][Z] );
		s = 0.5 / r;
		result[Y] = 0.5 * r;
		result[X] = ( ortho[Y][X] + ortho[X][Y] ) * s;
		result[M] = ( ortho[Z][X] - ortho[X][Z] ) * s;
		result[Z] = ( ortho[Z][Y] + ortho[Y][Z] ) * s;
	}
	else {
		r = sqrt( 1.0 + ortho[Z][Z] - ortho[X][X] - ortho[Y][Y] );
		s = 0.5 / r;
		result[Z] = 0.5 * r;
		result[X] = ( ortho[Z][X] + ortho[X][Z] ) * s;
		result[Y] = ( ortho[Z][Y] + ortho[Y][Z] ) * s;
		result[M] = ( ortho[X][Y] - ortho[Y][X] ) * s;
	}
#endif
}


void VectorsMixin::SetQuaterniond( Quaternion result, double degrees, const Vector3 axis ) {
	SetQuaternion( result, degrees * pi / 180.0, axis );
}

void VectorsMixin::ComputeQuaternionConjugate( Quaternion conjugate, Quaternion q ) {
	conjugate[M] = q[M]; ScaleVector( conjugate, q, -1.0 );
}

double VectorsMixin::QuaternionDifference( Quaternion result, const Quaternion q1, const Quaternion q2 ) {
	Quaternion conjugate;
	double angle;
	// Compute q1 q2*.
	conjugate[M] = q2[M]; ScaleVector( conjugate, q2, -1.0 );
	MultiplyQuaternions( result, q1, conjugate );
	angle = 2.0 * atan2( VectorNorm( result ), result[M] );
	return( angle );
}

double VectorsMixin::AngleBetweenOrientations( const Quaternion q1, const Quaternion q2 ) {
	Quaternion discard;
	return( QuaternionDifference( discard, q1, q2 ) );

}

double VectorsMixin::RotationAngle( const Quaternion q1 ) {
	double hypotenuse = sqrt( q1[X] * q1[X] + q1[Y] * q1[Y] +  q1[Z] * q1[Z] );
	return( 2.0 * atan2( hypotenuse, q1[M] ) );
}

// Compute the roll component of an arbitray rotation expressed as a quaternion.
// Here we consider roll to be around the Z axis.
double VectorsMixin::RollAngle( const Quaternion q1 ) {

	Vector3	sight_axis;
	Vector3 pitch_yaw_axis;
	double pitch_yaw_rotation_angle;
	Quaternion pitch_yaw_transformation;
	Vector3 up_vector1, up_vector2, up_cross;

	RotateVector( sight_axis, q1, kVector );
	ComputeCrossProduct( pitch_yaw_axis, kVector, sight_axis );
	NormalizeVector( pitch_yaw_axis );
	pitch_yaw_rotation_angle = AngleBetweenVectors( sight_axis, kVector );
	pitch_yaw_transformation[X] = sin( pitch_yaw_rotation_angle / 2.0 ) * pitch_yaw_axis[X];
	pitch_yaw_transformation[Y] = sin( pitch_yaw_rotation_angle / 2.0 ) * pitch_yaw_axis[Y];
	pitch_yaw_transformation[Z] = sin( pitch_yaw_rotation_angle / 2.0 ) * pitch_yaw_axis[Z];
	pitch_yaw_transformation[M] = cos( pitch_yaw_rotation_angle / 2.0 );

	RotateVector( up_vector1, pitch_yaw_transformation, jVector );
	RotateVector( up_vector2, q1, jVector );
	double angle = acos( DotProduct( up_vector1, up_vector2 ) );
	ComputeCrossProduct( up_cross, up_vector1, up_vector2 );
	if ( DotProduct( up_cross, sight_axis ) > 0.0 ) return( angle );
	else return( - angle );
}

void VectorsMixin::TransformPose( Pose &result, Transform &xform, Pose &source ) {
	Vector3 x;
	Quaternion q;
	MultiplyQuaternions( q, xform.rotation, source.orientation );
	CopyQuaternion( result.orientation, q );
	RotateVector( x,  xform.rotation, source.position );
	AddVectors( result.position, x, xform.displacement );
}

/***********************************************************************************/

// Compute the position and orientation of a rigid body defined by 3D marker locations.
// Takes as input the positions of the markers when the rigid body is at the zero
// position and orientation, and a matching list of the current position of the markers.

// We assume that the caller has created the list from visible marker data only.

// Takes also an optional 'default' orientation that is used when only 1 or 2 markers
// are provided, to compute the position.

bool VectorsMixin::ComputeRigidBodyPose( Vector3 position, Quaternion orientation,
										 Vector3 model[], Vector3 actual[], int N, const Quaternion default_orientation ) {

	Vector3		model_centroid, actual_centroid;
	Vector3		model_delta[MAX_RIGID_BODY_MARKERS], actual_delta[MAX_RIGID_BODY_MARKERS];
	Vector3		model_center_of_rotation, actual_center_of_rotation;
	Matrix3x3	model_local, actual_local;

	int i;

	if (N > MAX_RIGID_BODY_MARKERS) N = MAX_RIGID_BODY_MARKERS;

	bool valid = false;

	// Need at least 1 marker if a default orientation is given, and
	// at least 3 markers if no default orientation is given.

	if ( N < 3 && ! default_orientation ) {
		position[X] = position[Y] = position[Z] = -999.999;
		orientation[X] = orientation[Y] = orientation[Z] = orientation[M] = -999.999;
		return( false );
	}

	// If we have more than 3 markers, the system is over-constrained.
	// We compute the rotation between model and actual that best fits the data.
	if ( N > 3 ) {

		// Compute the centroid of the input and output vectors;
		CopyVector( model_centroid, zeroVector );
		CopyVector( actual_centroid, zeroVector );
		for ( i = 0; i < N; i++ ) {
			AddVectors( model_centroid, model_centroid, model[i] );
			AddVectors( actual_centroid, actual_centroid, actual[i] );
		}
		ScaleVector( model_centroid, model_centroid, 1.0 / (double) N );
		ScaleVector( actual_centroid, actual_centroid, 1.0 / (double) N );

		// Now compute the vector offset of each marker from the centroid.
		for ( i = 0; i < N; i++ ) {
			SubtractVectors( model_delta[i], model[i], model_centroid );
			SubtractVectors( actual_delta[i], actual[i], actual_centroid );
		}

		// If the vectors are co-planar, we can get a degenerate solution.
		// The following makes sure that the center-of-rotation is out of
		// that plane, avoiding the problem.

		// Move perpendicular to the plane defined by the first two deltas.
		ComputeCrossProduct( model_center_of_rotation, model_delta[0], model_delta[1] );
		// Make the displacement off the plane about the same magnitude as the other deltas from the centroid.
		ScaleVector( model_center_of_rotation, model_center_of_rotation, VectorNorm( model_delta[0] ) / VectorNorm( model_delta[0] ) / VectorNorm( model_delta[01] ) );
		// Do the same for the actual, measured marker positions.
		ComputeCrossProduct( actual_center_of_rotation, actual_delta[0], actual_delta[1] );
		// The 'model_delta[0] in the next line is not a mistake. I want the center of rotation to have precisely the same length in both cases.
		ScaleVector( actual_center_of_rotation, actual_center_of_rotation, VectorNorm( model_delta[0] ) / VectorNorm( actual_delta[0] ) / VectorNorm( actual_delta[01] ) );
		for ( i = 0; i < N; i++ ) {
			SubtractVectors( model_delta[i], model_delta[i], model_center_of_rotation );
			SubtractVectors( actual_delta[i], actual_delta[i], actual_center_of_rotation );
		}

		// Compute the best fit transformation between the two.
		Matrix3x3 best, ortho_best;
		BestFitTransformation( best, model_delta, actual_delta, N );
		OrthonormalizeMatrix( ortho_best, best );
		MatrixToQuaternion( orientation, ortho_best );

	}

	// If we have only 3 markers, then the system is exactly constrained for 
	// a rotation. We compute the matrix that maps model to actual.
	else if ( N == 3 ) {

		Vector3 temp;
		Matrix3x3 inverse;
		Matrix3x3 exact;

		// Use one relative vector to define the X axis.
		SubtractVectors( model_local[X], model[1], model[0] );
		NormalizeVector( model_local[X] );
		// The second relative vector lies in the XY plane.
		SubtractVectors( temp, model[2], model[0] );
		// Use these two to compute the Z vector;
		ComputeCrossProduct( model_local[Z], model_local[X], temp );
		NormalizeVector( model_local[Z] );
		// Now compute the Y vector.
		ComputeCrossProduct( model_local[Y], model_local[Z], model_local[X] );
		NormalizeVector( model_local[Y] );

		// Use one relative vector to define the X axis.
		SubtractVectors( actual_local[X], actual[1], actual[0] );
		NormalizeVector( actual_local[X] );
		// The second relative vector lies in the XY plane.
		SubtractVectors( temp, actual[2], actual[0] );
		// Use these two to compute the Z vector;
		ComputeCrossProduct( actual_local[Z], actual_local[X], temp );
		NormalizeVector( actual_local[Z] );
		// Now compute the Y vector perpendicular to the XZ pair.
		ComputeCrossProduct( actual_local[Y], actual_local[Z], actual_local[X] );
		NormalizeVector( actual_local[Y] );

		InvertMatrix( inverse, model_local );
		MultiplyMatrices( exact, inverse, actual_local );
		MatrixToQuaternion( orientation, exact );

	}

	// If we have 2 or fewer markers, then we cannot compute an orientation.
	// But we need to have an orientation to correctly compute the position.
	// If the caller has specified a default orientation, we use it, and
	// then compute the position (displacement from zero) as for the other cases.
	else {

		CopyQuaternion( orientation, default_orientation );

	}

	if ( N == 8 ) {
		i = i;
	}
	// To compute the position (displacement) we first calculate the model 
	// marker positions at the actual orientation, then we compute the 
	// average difference between rotated model and actual to get the displacement.
	CopyVector( position, zeroVector );
	for ( i = 0; i < N; i++ ) {
		Vector3 rotated_model;
		Vector3 offset;

		// Ideal position of the marker when rotated around zero.
		RotateVector( rotated_model, orientation, model[i] );
		// Difference between actual and rotated model.
		SubtractVectors( offset, actual[i], rotated_model );
		// Compute the sum on the way to computing the average.
		AddVectors( position, position, offset );
	}
	ScaleVector( position, position, 1.0 / (double) N );

	return( true );

}

/***********************************************************************************/

// Compute a set of angles that is representative of the rotation of an object
//  away from the null orientation around each of the X, Y and Z axies. 
// This set of 3 angles is NOT a set of Euler angles. You cannot reconstruct the
//  final 3D orientation by concatenating these 3 rotations. 
// Rather, it is the first rotation of each of 3 sets of Euler angles. I find it
//  as an intuitive way to represent the attitude of an object.
 
void VectorsMixin::QuaternionToCannonicalRotations( Vector3 r, Quaternion q ) {
	
	r[X] = atan2(2 * (q[M] * q[X] + q[Y] * q[Z]), 1.0 - (q[X] * q[X] + q[Y] * q[Y]));
	r[Y] = atan2(2 * (q[M] * q[Y] + q[X] * q[Z]), 1.0 - (q[Y] * q[Y] + q[Z] * q[Z]));
	r[Z] = atan2(2 * (q[M] * q[Z] + q[X] * q[Y]), 1.0 - (q[X] * q[X] + q[Z] * q[Z]));

}
										
/***********************************************************************************/

// These routines create ascii strings from vector and matrix objects.
// They are useful for displaying the values of these object via printf();

char *VectorsMixin::vstr( const Vector3 v, const char *format ) {

	// This is a circular buffer of static strings.
	// Each time this routine is calle we use a different string to hold the result.
	// That way, if the routine gets called multiple times in a single printf(), 
	//  for example, one call does not write over the output of another.
	// This assumes that we will never need the results of more than 256 at the same time.

	static char str[256][256];
	static int instance = 0;
	instance++;
	instance %= 256;

	// Create the string here.
	sprintf( str[instance], format, v[X], v[Y], v[Z] );

	return( str[instance] );

}
char *VectorsMixin::vstr( const fVector3 v, const char *format ) {
	Vector3 d;
	CopyVector( d, v );
	return( vstr( d, format ) );
}


// The others work in a similar fashion.
char *VectorsMixin::qstr( const Quaternion q, const char *format ) {
	static char str[256][256];
	static int instance = 0;
	instance++;
	instance %= 256;
	sprintf( str[instance], format, q[X], q[Y], q[Z], q[M] );
	return( str[instance] );
}
char *VectorsMixin::qstr( const fQuaternion q, const char *format ) {
	Quaternion d;
	CopyQuaternion( d, q );
	return( qstr( d, format ) );
}

char *VectorsMixin::mstr( const Matrix3x3 m, const char *format ) {
	static char str[256][256];
	static int instance = 0;
	instance++;
	instance %= 256;
	sprintf( str[instance], format, 
		m[X][X], m[Y][X], m[Z][X], m[X][Y], m[Y][Y], m[Z][Y], m[X][Z], m[Y][Z], m[Z][Z] );
	return( str[instance] );
}
char *VectorsMixin::mstr( const fMatrix3x3 m, const char *format ) {
	Matrix3x3 d;
	CopyMatrix( d, m );
	return( mstr( d, format ) );
}
