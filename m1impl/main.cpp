#include "../margaret1/system.h"
#include "../margaret1/table.h"
#include "../margaret1/law.h"

using namespace std;
using namespace margaret1;

constexpr double constR = 8.314462618; // universal gas constant

// Aerodynamic relations and properties relevant to the research at hand

Property    T({"T"}),
            T0({"T0"}),
            T0_T({"T0_T"}),
            h0({"h0"}),
            J({"J"}),
            p({"p"}),
            p0({"p0"}),
            p0_p({"p0_p"}),
            rho({"rho"}),
            rho0({"rho0"}),
            rho0_rho({"rho0_rho"}),
            M({"M"}),
            u({"u"}),
            c({"c"}),
            G({"G"}),
            A({"A"}),
            R({"R"}),
            dotm({"dotm"}),
            mu({"mu"}),
            gam({"gam"});

vector<Property> canonicalProperties = {
    T, T0, h0, J, p, p0, rho, rho0, 
    M, u, c, G, A, R, dotm, mu, gam
};

MultLaw L1("L1", {{"u", 1}, {"M", -1}, {"c", -1}});                                     // u = M c
MultLaw L2("L2", {{"c", 2}, {"gam", -1}, {"R", -1}, {"T", -1}, {"mu", 1}});           // c^2 = gam R T / mu
MultLaw L3("L3", {{"G", 1}, {"rho", -1}, {"u", -1}});                                   // G = rho u
MultLaw L4("L4", {{"dotm", 1}, {"A", -1}, {"rho", -1}, {"u", -1}});                     // dotm = A rho u
MultLaw L5("L5", {{"p", 1}, {"mu", 1}, {"rho", -1}, {"R", -1}, {"T", -1}});             // p / rho = R T / mu
MultLaw L6("L6", {{"G", 1}, {"dotm", -1}, {"A", -1}});
LawSet Ls({&L1, &L2, &L3, &L4, &L5, &L6});

Relation R1("R1", "T0_T", SEQ(1 + ((P(gam) - 1)*P(M)*P(M)/2)), {"M", "gam"});       // T0/T = 1 + (gam-1)M^2 /2 
Relation R2("R2", "G", SEQ(
    P(J)*P(M) * sqrt((pow(P(gam), 2)/(P(gam)-1) * P(T0_T)) / P(h0)) / (1 + P(gam)*pow(P(M), 2))
), {"J", "M", "h0", "gam", "T0_T"});

vector<Relation> getCanonicalRelations() {
    vector<Relation> res = {R1, R2};
    for (Relation rel: Ls.deriveAllRelations()) res.push_back(rel);
    return res;
}

int main() {
    vector<Relation> canon = getCanonicalRelations();
    margaretData("Relations loaded", {(double)(canon.size())});
}