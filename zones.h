// GROUP( grupp index/#, antal punkter, %-chans att man visar mer än ett mål, %-chans att man ska hålla mål );

// ZONE( "GRUPP", "TAG", "KART", "hjälp rad 1", "hjälp rad 2" );
// END;

BEGINGROUPS(6)// Glöm inte att ändra denna siffra så den stämmer övens med antalet grupper
GROUP( 0, 100, 0 )
GROUP( 1, 0, 100 )
GROUP( 2, 100, 0 )
GROUP( 3, 0, 100 )
GROUP( 4, 100, 0 )
GROUP( 5, 0, 100 ) // sista är slutspel
ENDGROUPS()

BEGINZONES(39) // Glöm inte att ändra denna siffra så den stämmer övens med antalet zoner
ZONE( 0, "TAG17", "0-01", "       Leta i       ", "       rummet       ")
ZONE( 0, "TAG02", "1-31", "       Leta i       ", "       rummet       ")
ZONE( 0, "TAG65", "1-18", "       Leta i       ", "       rummet       ")
ZONE( 0, "TAG08", "2-32", "       Leta i       ", "       rummet       ")
ZONE( 0, "TAG32", "2-45", "       Leta i       ", "       rummet       ")
ZONE( 0, "TAG70", "3-62", "       Leta i       ", "       rummet       ")
ZONE( 0, "TAG48", "3-53", "       Leta i       ", "       rummet       ")

ZONE( 1, "TAG36", "0-04", "       Leta i       ", "       rummet       ")
ZONE( 1, "TAG64", "1-21", "       Leta i       ", "       rummet       ")
ZONE( 1, "TAG05", "1-27", "       Leta i       ", "       rummet       ")
ZONE( 1, "TAG53", "2-36", "       Leta i       ", "       rummet       ")
ZONE( 1, "TAG28", "2-49", "       Leta i       ", "       rummet       ")
ZONE( 1, "TAG13", "3-60", "       Leta i       ", "       rummet       ")
ZONE( 1, "TAG44", "3-67", "       Leta i       ", "       rummet       ")

ZONE( 2, "TAG24", "0-02", "       Leta i       ", "       rummet       ")
ZONE( 2, "TAG69", "1-25", "       Leta i       ", "       rummet       ")
ZONE( 2, "TAG57", "1-09", "       Leta i       ", "       rummet       ")
ZONE( 2, "TAG16", "2-33", "       Leta i       ", "       rummet       ")
ZONE( 2, "TAG60", "2-42", "       Leta i       ", "       rummet       ")
ZONE( 2, "TAG10", "3-54", "       Leta i       ", "       rummet       ")
ZONE( 2, "TAG38", "3-63", "       Leta i       ", "       rummet       ")

ZONE( 3, "TAG62", "0-05", "       Leta i       ", "       rummet       ")
ZONE( 3, "TAG18", "1-20", "       Leta i       ", "       rummet       ")
ZONE( 3, "TAG29", "1-29", "       Leta i       ", "       rummet       ")
ZONE( 3, "TAG23", "2-52", "       Leta i       ", "       rummet       ")
ZONE( 3, "TAG49", "2-50", "       Leta i       ", "       rummet       ")
ZONE( 3, "TAG03", "3-62", "       Leta i       ", "       rummet       ")
ZONE( 3, "TAG30", "3-59", "       Leta i       ", "       rummet       ")

ZONE( 4, "TAG45", "0-08", "       Leta i       ", "       rummet       ")
ZONE( 4, "TAG68", "1-12", "       Leta i       ", "       rummet       ")
ZONE( 4, "TAG56", "1-15", "       Leta i       ", "       rummet       ")
ZONE( 4, "TAG27", "2-38", "       Leta i       ", "       rummet       ")
ZONE( 4, "TAG61", "2-43", "       Leta i       ", "       rummet       ")
ZONE( 4, "TAG07", "3-55", "       Leta i       ", "       rummet       ")  
ZONE( 4, "TAG42", "3-65", "       Leta i       ", "       rummet       ")


ZONE( 5, "END89", "????", "Plan 0", "Under trappan")
ZONE( 5, "END88", "????", "N trapphuset, mellan", "Plan 1 och Plan 2")
ZONE( 5, "END84", "????", "Vid hissen, plan 2", "Lycka till....")
ZONE( 5, "END87", "????", "Rum 62", "Trappdorren ")

ENDZONES()












