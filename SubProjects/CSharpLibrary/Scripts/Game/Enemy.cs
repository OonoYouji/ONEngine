using System;

/// <summary>
/// 敵のクラス。HP管理とシンプルなパトロールAIを持つ。
/// </summary>
public class Enemy : MonoScript {
    [SerializeField] float hp = 100f;
    [SerializeField] float moveSpeed = 3.0f;
    [SerializeField] float patrolRadius = 10.0f;

    private bool isAlive = true;
    private Vector3 startPosition;
    private Vector3 targetPosition;
    private Random random = new Random();

    public override void Initialize() {
        startPosition = transform.position;
        SetNewTarget();
    }

    public override void Update() {
        if (hp <= 0f && isAlive) {
            isAlive = false;
            Debug.Log("Enemy defeated: " + entity.Id);
            // 本来はここで死亡アニメーションやエンティティの削除を行う
            return;
        }

        if (!isAlive) return;

        // パトロール移動
        Patrol();
    }

    private void Patrol() {
        Vector3 currentPos = transform.position;
        Vector3 direction = targetPosition - currentPos;
        
        if (direction.Length() < 0.5f) {
            SetNewTarget();
        } else {
            direction = direction.Normalized();
            transform.position += direction * moveSpeed * Time.deltaTime;
            
            // 進行方向に向く
            float rotateY = Mathf.Atan2(direction.z, direction.x);
            Vector3 euler = new Vector3(0, -rotateY + Mathf.PI / 2.0f, 0);
            transform.rotate = Quaternion.FromEuler(euler);
        }
    }

    private void SetNewTarget() {
        float angle = (float)random.NextDouble() * Mathf.PI * 2.0f;
        float distance = (float)random.NextDouble() * patrolRadius;
        
        targetPosition = startPosition + new Vector3(
            Mathf.Cos(angle) * distance,
            0,
            Mathf.Sin(angle) * distance
        );
    }

    public bool IsAlive { 
        get { return isAlive; }
    }

    public void TakeDamage(float damage) {
        hp -= damage;
    }
}
