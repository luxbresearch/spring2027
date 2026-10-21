#include "../margaret1/system.h"
#include "../margaret1/table.h"

using namespace std;
using namespace margaret1;

constexpr double R = 8.314462618; // universal gas constant

// Aerodynamic relations and properties relevant to the research at hand

Property    T({"T"}),
            T0({"T0"}),
            h0({"h0"}),
            J({"J"}),
            p({"p"}),
            p0({"p0"}),
            rho({"rho"}),
            rho0({"rho0"}),
            M({"M"}),
            c("c"),
            G({"G"}),
            A({"A"}),

            dotm({"dotm"}),
            mu({"mu"}),
            gamma({"gamma"});