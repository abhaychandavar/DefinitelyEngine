#include "depch.h"
#include "CollisionWorld.h"

namespace DefinitelyEngine {

    void CollisionWorld::Register(Collider* collider) {
        m_Colliders.push_back(collider);
    }

    void CollisionWorld::Unregister(const std::string& ownerName) {
        m_Colliders.erase(
            std::remove_if(m_Colliders.begin(), m_Colliders.end(),
                [&](Collider* c) { return c->ownerName == ownerName; }),
            m_Colliders.end());
    }

    Collider* CollisionWorld::Find(const std::string& ownerName) const {
        for (Collider* c : m_Colliders)
            if (c->ownerName == ownerName) return c;
        return nullptr;
    }

    void CollisionWorld::IgnoreCollision(const std::string& tagA, const std::string& tagB) {
        auto pair = (tagA <= tagB) ? std::make_pair(tagA, tagB) : std::make_pair(tagB, tagA);
        m_IgnorePairs.push_back(pair);
    }

    bool CollisionWorld::ShouldIgnore(const std::string& tagA, const std::string& tagB) const {
        auto pair = (tagA <= tagB) ? std::make_pair(tagA, tagB) : std::make_pair(tagB, tagA);
        for (const auto& p : m_IgnorePairs)
            if (p == pair) return true;
        return false;
    }

    // ---------------------------------------------------------------------------
    // Private helper: capsule-capsule overlap (both Y-axis aligned).
    // outNormal points from b's closest point toward a's closest point.
    static bool TestCapsuleCapsule(const CapsuleCollider& a, const CapsuleCollider& b,
                                   glm::vec3& outNormal, float& outDepth)
    {
        float aYMin = a.worldCenter.y - a.height * 0.5f;
        float aYMax = a.worldCenter.y + a.height * 0.5f;
        float bYMin = b.worldCenter.y - b.height * 0.5f;
        float bYMax = b.worldCenter.y + b.height * 0.5f;

        // Closest point on each segment to the other segment (parallel Y-axis segments).
        glm::vec3 closestA = { a.worldCenter.x, glm::clamp(b.worldCenter.y, aYMin, aYMax), a.worldCenter.z };
        glm::vec3 closestB = { b.worldCenter.x, glm::clamp(a.worldCenter.y, bYMin, bYMax), b.worldCenter.z };

        glm::vec3 diff = closestA - closestB;
        float dist     = glm::length(diff);
        float minDist  = a.radius + b.radius;

        if (dist >= minDist) return false;

        outDepth  = minDist - dist;
        outNormal = (dist < 1e-6f) ? glm::vec3(1.0f, 0.0f, 0.0f) : diff / dist;
        return true;
    }

    void CollisionWorld::Update() {
        m_Contacts.clear();

        for (size_t i = 0; i < m_Colliders.size(); ++i) {
            for (size_t j = i + 1; j < m_Colliders.size(); ++j) {
                Collider* ca = m_Colliders[i];
                Collider* cb = m_Colliders[j];

                if (ShouldIgnore(ca->tag, cb->tag)) continue;

                // Capsule vs Capsule only — ground plane collision is handled by raycasts.
                if (ca->GetType() != ColliderType::Capsule) continue;
                if (cb->GetType() != ColliderType::Capsule) continue;

                glm::vec3 normal;
                float     depth;
                if (TestCapsuleCapsule(
                        *static_cast<CapsuleCollider*>(ca),
                        *static_cast<CapsuleCollider*>(cb),
                        normal, depth))
                {
                    m_Contacts.push_back({ ca, cb, normal, depth });
                }
            }
        }
    }

    // ---------------------------------------------------------------------------
    // Private helper: ray vs AABB slab method.
    // Returns true on hit; populates distance and normal.
    static bool TestBox(const BoxCollider& box, const Ray& ray, float maxDist,
                        RaycastHit& outHit) {
        glm::vec3 minB = box.worldCenter - box.halfExtents;
        glm::vec3 maxB = box.worldCenter + box.halfExtents;

        float tEnter = 0.0f;
        float tExit  = maxDist;
        int   hitAxis = -1;

        for (int i = 0; i < 3; i++) {
            float d = ray.direction[i];
            float o = ray.origin[i];

            if (glm::abs(d) < 1e-8f) {
                // Ray parallel to slab — miss if outside
                if (o < minB[i] || o > maxB[i]) return false;
            } else {
                float t1 = (minB[i] - o) / d;
                float t2 = (maxB[i] - o) / d;
                if (t1 > t2) std::swap(t1, t2);
                if (t1 > tEnter) { tEnter = t1; hitAxis = i; }
                tExit = std::min(tExit, t2);
                if (tEnter > tExit) return false;
            }
        }

        if (tExit < 0.0f) return false;

        outHit.distance = tEnter >= 0.0f ? tEnter : tExit;
        if (outHit.distance > maxDist) return false;

        outHit.point = ray.origin + ray.direction * outHit.distance;

        // Compute outward-facing normal from the entry axis
        outHit.normal = glm::vec3(0.0f);
        if (hitAxis >= 0) {
            outHit.normal[hitAxis] = ray.direction[hitAxis] < 0.0f ? 1.0f : -1.0f;
        }
        return true;
    }

    static bool TestCapsule(const CapsuleCollider& capsule, const Ray& ray, float maxDist,
                            RaycastHit& outHit) {
        const glm::vec3 base = capsule.worldCenter - glm::vec3(0.0f, capsule.height * 0.5f, 0.0f);
        const glm::vec3 top  = capsule.worldCenter + glm::vec3(0.0f, capsule.height * 0.5f, 0.0f);
        const glm::vec3 segment = top - base;
        const glm::vec3 fromBaseToOrigin = ray.origin - base;

        const float a = glm::dot(ray.direction, ray.direction);
        const float b = glm::dot(ray.direction, segment);
        const float c = glm::dot(segment, segment);
        const float d = glm::dot(ray.direction, fromBaseToOrigin);
        const float e = glm::dot(segment, fromBaseToOrigin);
        const float denom = a * c - b * b;

        float rayT = 0.0f;
        float segT = 0.0f;

        if (glm::abs(denom) > 1e-6f) {
            rayT = (b * e - c * d) / denom;
            segT = (a * e - b * d) / denom;
        } else {
            segT = e / glm::max(c, 1e-6f);
        }

        rayT = glm::max(rayT, 0.0f);
        segT = glm::clamp(segT, 0.0f, 1.0f);

        const glm::vec3 closestRayPoint = ray.origin + ray.direction * rayT;
        const glm::vec3 closestSegmentPoint = base + segment * segT;
        const glm::vec3 delta = closestRayPoint - closestSegmentPoint;
        const float distSq = glm::dot(delta, delta);
        const float radiusSq = capsule.radius * capsule.radius;

        if (distSq > radiusSq)
            return false;

        const float reach = glm::sqrt(glm::max(radiusSq - distSq, 0.0f));
        float hitDistance = rayT - reach;
        if (hitDistance < 0.0f)
            hitDistance = rayT + reach;

        if (hitDistance < 0.0f || hitDistance > maxDist)
            return false;

        outHit.distance = hitDistance;
        outHit.point = ray.origin + ray.direction * hitDistance;

        glm::vec3 projectedPoint = outHit.point;
        projectedPoint.y = glm::clamp(projectedPoint.y, base.y, top.y);
        glm::vec3 normal = outHit.point - projectedPoint;
        if (glm::dot(normal, normal) < 1e-6f) {
            normal = glm::vec3(0.0f, 1.0f, 0.0f);
        } else {
            normal = glm::normalize(normal);
        }
        outHit.normal = normal;
        return true;
    }

    bool CollisionWorld::Raycast(const Ray& ray, float maxDist, RaycastHit& outHit, const std::string& excludeTag) const {
        float      bestDist = maxDist;
        RaycastHit bestHit  = {};
        bool       anyHit   = false;

        for (Collider* c : m_Colliders) {
            if (!excludeTag.empty() && c->tag == excludeTag) continue;
            if (c->GetType() == ColliderType::Capsule) {
                // TODO: ray vs capsule not yet implemented — capsules are transparent to raycasts.
                // Future: infinite-cylinder test + hemispherical end-cap tests.
                continue;
            }

            auto* box = static_cast<BoxCollider*>(c);
            RaycastHit candidate;
            if (TestBox(*box, ray, bestDist, candidate)) {
                candidate.collider = c;
                candidate.tag      = c->tag;
                bestDist = candidate.distance;
                bestHit  = candidate;
                anyHit   = true;
            }
        }

        if (anyHit) { outHit = bestHit; return true; }
        return false;
    }

}
