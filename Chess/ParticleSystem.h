#pragma once
#include"DataStructure/Vector3D.h"

typedef Vector3D<float> TV; //Template Vector

class Particle {
public:
	Vector3D<float> pos, vel, acc;
	float mass = 1.0f;
	float rand;
};

class Sphere {
public: // f(x) = (x^2-xc)^2 + (y^2-yc)^2 + (z^2-zc)^2 -r^2
	TV center;
	float radius;
	TV getNormal(const TV& pos) {
		return (pos - center).getNormalized();
	}
	float getSignedDistance(const TV& pos) { // less than 0 = collision
		return (pos - center).getMagnitude() - radius;
	}
};

class ParticleSystem {
public:
	std::vector<Particle> particles;
	//Sphere obj;
	
	void initParticleSystem(const int num_particle, const float& z, const float& x) {
		//obj.center = TV(z,x, 0.5);
		//obj.radius = 0.3;


		particles.resize(num_particle);

		for (int p = 0; p < particles.size(); ++p) {
			TV& pos(particles[p].pos);
			TV& vel(particles[p].vel);
			float& m = particles[p].mass;

			pos.x_ = x;
			pos.y_ = 0;
			pos.z_ = z;
			//pos.x_ = (float)rand() / (float)RAND_MAX; // [0,1)
			//pos.y_ = (float)rand() / (float)RAND_MAX + 0.9; // [0,1)
			//pos.z_ = (float)rand() / (float)RAND_MAX; // [0,1)
			m = (float)rand() / (float)RAND_MAX *10;


			particles[p].rand = (float)rand() / (float)RAND_MAX;
			if (particles[p].rand <= 0.7) {
				particles[p].rand += 0.7;
			}
		}
	}
	
	// update
	void advanceOneTimeStep(const float& dt) { // delta time
		TV g = TV(0.0, -9.8, 0.0);
		
		const float A = 3.14 * 1 * 1;
		const float C = 0.47; // Drag Coefficient


		for (int p = 0; p < particles.size(); ++p) {
			TV& pos(particles[p].pos);
			TV& vel(particles[p].vel);
			
			float mag = sqrt(pow(vel.x_, 2) + pow(vel.x_, 2) + pow(vel.x_, 2));
			TV F_air = -0.5f * A * C * mag * vel;

			//g += F_air;

			//force
			TV f = particles[p].mass * g;//* particles[p].rand;// g=a

			// force -> velocity
			vel += (f / particles[p].mass)*dt;
			// velocity -> new position
			pos += vel * dt;
		}

		const float coef = 0.3;

		for (int p = 0; p < particles.size(); p++) {
			TV& pos(particles[p].pos);
			TV& vel(particles[p].vel);
			
			if (pos.y_ < 0.0f) { // ground collision
				if (vel.y_ < 0.0f) {
					vel.y_ *= -coef;
					//vel.y_ *= -1.0; // perfect reflection
				}
			}
		}


		// object collision
		//const float frac = 0.3;

		//for (int p = 0; p < particles.size(); p++) {
		//	TV& pos(particles[p].pos);
		//	TV& vel(particles[p].vel);

		//	// collision
		//	if (obj.getSignedDistance(pos) <= 0.0f) {
		//		const TV normal = obj.getNormal(pos);
		//		const float v_dot_norm = dotProduct(vel, normal);

		//		if (v_dot_norm < 0.0f) { // collision occured
		//			const TV v_n = v_dot_norm * normal;
		//			const TV v_t = vel - v_n;

		//			vel = v_t* frac + (-coef)*v_n;
		//		}
		//	}
		//}
	}
};