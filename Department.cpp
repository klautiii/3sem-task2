#include "Department.h"

std::string DepartmentName(Department department) {
    switch (department) {
    case Department::Men:    return "Мужская обувь";
    case Department::Women:  return "Женская обувь";
    case Department::Kids:   return "Детская обувь";
    case Department::Sports: return "Спортивная обувь";
    }
    return "Неизвестный отдел";
}

const std::vector<Department>& AllDepartments() {
    static const std::vector<Department> kAllDepartments = {
        Department::Men,
        Department::Women,
        Department::Kids,
        Department::Sports,
    };
    return kAllDepartments;
}
