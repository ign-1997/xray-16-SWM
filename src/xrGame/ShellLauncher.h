#pragma once

/********************************/
/***** Запускатель 3D-Гильз *****/ //--#SM+#--
/********************************/

class CCustomShell;
class CGameObject;

#define SHELL3D_DEF_LIFETIME 5000 // Время существования гильзы по умолчанию
#define SHELL3D_DEF_COL_LIFETIME 0 // Минимальное время, раньше которого не можем удалять гильзу при столкновениях
#define SHELL3D_PARENT_DEF_SPEED_FACTOR 1.35f // Дефолтный модификатор вектора скорости владельца, прибавляемой к гильзе
#define SHELL3D_CAM_MAGNITUDE_F 0.8f // Фактор влияния поворота камеры на скорость полёта гильзы вбок

class CShellLauncher
{
    friend CCustomShell;

protected: // Abstract <!>
    virtual void Update3DShellTransform() = 0; // Получить текущую целевую позицию 3D-гильзы на оружии

public:
    // Детальные параметры конкретной точки запуска
    struct _lpoint
    {
    private:
        friend CShellLauncher;

    public:
        //--> Начальные параметры запуска гильз из конфига
        bool bUseBoneDir; // Использовать дирекцию кости при спавне гильзы
        shared_str sBoneName; // Кость, относительно которой спавним гильзу (+ анимация)

        bool bAnimatedLaunch; // Использовать анимированный запуск гильзы
        u32 dwAnimReleaseTime; // Время (в м\сек) через которое включится физика после анимации
        u32 dwAnimReleaseTimePump; // Время (в м\сек) через которое включится физика после анимации (для помпового режима)

        float fParentVelFactor; // Фактор влияния скорости владельца на направление полёта гильзы

        u32 dwLifeTime; // Время (в м\сек) после которого удалим гильзу (0 - не удаляем)

        bool bDestroyOnCollide; // Уничтожить гильзу при колллизии с геометрией
        u32 dwMinCollideLifeTime; // Время (в м\сек) раньше которого не удаляем гильзу при столкновениях

        Fvector vOfsPos; // Смещение начальной позиции гильзы относительно кости
        Fvector vOfsDir; // Смещение начального поворота гильзы относительно кости (в градусах)
        Fvector vOfsDirRnd; // Лимиты случайного отклонения начального поворота (в градусах)
        Fvector vVelocity; // Вектор полёта после спавна относительно кости
        Fvector vVelocityRnd; // Лимиты случайного отклонения начального вектора полёта
        /*===================*/

    private:
        //--> Обработанные параметры запуска, которые будут использованы гильзой
        bool bEnabled; // Флаг активности для данной точки
        u16 iBoneID; // ID кости, относительно которой спавним гильзу
        Fmatrix vLaunchMatrix; // Матрица трансформации при спавне, содержит текущий поворот и позицию
        Fvector vLaunchVel; // Текущий вектор полёта при спавне
        float fLaunchAVel; // Сила стартового "кручения" гильзы в полёте (угловая скорость)

    public:
        ICF bool IsEnabled() const { return bEnabled; }
        ICF u16 GetBoneID() const { return iBoneID; }
        ICF const Fmatrix& GetLaunchMatrix() const { return vLaunchMatrix; }
        ICF const Fvector& GetLaunchVel() const { return vLaunchVel; }
        ICF const float GetLaunchAVel() const { return fLaunchAVel; }

        _lpoint();
        _lpoint(const shared_str& sect_data, u32 _idx);
    };

    // Точка запуска гильз (мировая + худовая)
    struct launch_points
    {
        shared_str sShellOverSect; // Переопределённая секция гильзы (NULL - возмёт секцию из LaunchShell3D)
        u32 dwFOVTranslateTime; // Время (в м\сек) за которое FOV гильзы переходит из худового в мировой
        u32 dwFOVStableTime; // Время (в м\сек) в течении которого FOV гильзы ещё не меняется

        _lpoint point_world; // Параметры запуска гильзы для мировой модели
        _lpoint point_hud; // Параметры запуска гильзы для худовой модели

        launch_points(const shared_str& sWorldSect, const shared_str& sHudSect, u32 _idx);
    };

    CShellLauncher(CGameObject* parent);
    virtual ~CShellLauncher();

    DEFINE_VECTOR(launch_points, LAUNCH_VECTOR, LAUNCH_VECTOR_IT);
    LAUNCH_VECTOR m_launch_points; // Содержит все доступные точки запуска гильзы (для оружия с несколькими гильзами,
                                   // например БМ-16, ПКМ\ПКП)

    void LaunchShell3D(u32 launch_point_idx, LPCSTR sShellSect = nullptr)
        const; // Запустить гильзу из указанного launch_point (с возможностью указать свою секцию гильзы)

    ICF u32 GetLPCount() const { return m_launch_points_count; } // Узнать число точек запуска
    ICF CGameObject* GetParentObject() { return m_parent_shell_launcher; } // Получить владельца CShellLauncher

protected:
    shared_str m_params_sect; // Секция параметров запуска гильз для мировой модели
    shared_str m_params_hud_sect; // Секция параметров запуска гильз для худовой модели (если есть)
    CGameObject* m_parent_shell_launcher; // Владелец данного CShellLauncher
    u32 m_launch_points_count; // Число точек запуска

    void RegisterShell(u16 shell_id, CGameObject* parent_shell_launcher); // Зарегестрировать заспавненную гильзу

    void ReLoadLaunchPoints(
        const shared_str& sWorldSect, const shared_str& sHudSect = nullptr); // ПереЗагрузить параметры точек запуска гильз

    void RebuildLaunchParams(
        const Fmatrix& mTransform, IKinematics* pModel, bool bIsHud); // Пересчитать актуальные параметры точек запуска гильз

    virtual bool CanPlay3DShellAnim() const { return true; } // Можно-ли в данный момент запускать анимированные 3D-гильзы
};
