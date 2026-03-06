-- Mathlib v4.29.0-rc2
import Mathlib

open NumberTheorySymbols

def isSmooth (a k : ℕ) : Prop :=
∀ p : ℕ, Nat.Prime p → p ∣ a → p ≤ k

def isSumOfTwoSmooth (m k : ℕ) : Prop :=
∃ a b : ℕ, m = a + b ∧ isSmooth a k ∧ isSmooth b k ∧ a > 0 ∧ b > 0

def isPseudosquare (a : ℤ) (p : ℕ) : Prop :=
Nat.Prime p ∧ p ≠ 2 ∧ a ≡ 1 [ZMOD 8] ∧ J(a|p) = 1

lemma mod8_from_sum7 {a b : ℕ} (h : a + b ≡ 7 [MOD 8]) :
  a ≡ 7 - b % 8 [MOD 8] := by
  rw [Nat.ModEq] at h ⊢
  omega

lemma neg_mul_minus_1 (a : ℕ) : -(a:ℤ) = -1 * a := by
  simp only [Int.reduceNeg, neg_mul, one_mul]

lemma odd_minus_1_over_2 (A : ℕ) (ha_odd : Odd A) : A/2 = (A-1)/2 := by
  obtain ⟨k, rfl⟩ := ha_odd
  rw [<- Nat.div2_val, Nat.div2_bit1 k]
  simp

lemma neg_one_pow_of_1_mod_four {n : ℕ} (hn : n % 4 = 1) :
  (-1) ^ ((n - 1) / 2) = 1 := by
  rw [<- odd_minus_1_over_2]
  · exact ZMod.neg_one_pow_div_two_of_one_mod_four hn
  · grind only [=Nat.odd_iff]

lemma neg_one_pow_of_3_mod_four {n : ℕ} (hn : n % 4 = 3) :
  (-1) ^ ((n - 1) / 2) = -1 := by
  rw [<- odd_minus_1_over_2]
  · exact ZMod.neg_one_pow_div_two_of_three_mod_four hn
  · grind only [=Nat.odd_iff]

lemma jacobi_add_left (a c : ℤ) (p : ℕ) (h_div : (p : ℤ) ∣ c) :
  J(a + c| p) = J(a|p) := by
  nth_rewrite 1 [jacobiSym.mod_left]
  nth_rewrite 2 [jacobiSym.mod_left]
  rcases exists_eq_mul_right_of_dvd h_div with ⟨c, h_eq⟩
  simp only [h_eq, Int.add_mul_emod_self_left]

lemma jacobi_eq_prod_factorization (a : ℤ) (b : ℕ) (hb_ne0 : b ≠ 0) (hb_odd : Odd b) :
  J(a|b) = b.factorization.prod (fun p k => J(a|p) ^ k) := by
  induction b using induction_on_primes with
  | zero | one => simp
  | prime_mul p q h_prime h_ind =>
    have hp_ne0 : p ≠ 0 := left_ne_zero_of_mul hb_ne0
    have hq_ne0 : q ≠ 0 := right_ne_zero_of_mul hb_ne0
    haveI : NeZero p := of_not_not ((not_iff_not.mpr not_neZero).mpr hp_ne0)
    haveI : NeZero q := of_not_not ((not_iff_not.mpr not_neZero).mpr hq_ne0)
    rw [jacobiSym.mul_right, Nat.factorization_mul hp_ne0 hq_ne0]
    rw [Finsupp.prod_add_index (by simp), <- h_ind hq_ne0 (Nat.odd_mul.mp hb_odd).right]
    · simp only [Nat.Prime.factorization h_prime, pow_zero, Finsupp.prod_single_index, pow_one]
    · intros
      apply pow_add

lemma jacobi_2_pow_2_is_1 (B : ℕ) (hB : Odd B) : jacobiSym 2 B ^ 2 = 1 := by
  have h_jacobi_sq : J(2 | B) = 1 ∨ J(2 | B) = -1 := by
    obtain ⟨k, rfl⟩ : ∃ k, B = 2 * k + 1 := hB
    apply jacobiSym.eq_one_or_neg_one
    norm_num [Int.gcd]
  simp_all only [Int.reduceNeg, sq_eq_one_iff]

lemma jacobi_reciprocity {A B : ℕ} (ha_odd : Odd A) (hb_odd : Odd B) (hab_coprime : B.gcd A = 1)
  : J(A|B) * J(B|A) = (-1)^(((A-1)/2) * ((B-1)/2)) := by
  rw [jacobiSym.quadratic_reciprocity ha_odd hb_odd]
  rw [Int.mul_assoc, <- pow_two, jacobiSym.sq_one (by exact hab_coprime)]
  grind +ring only [odd_minus_1_over_2]

lemma sum_of_smooth_is_not_pseudosquare (m a b pn : ℕ)
  (h_pn : Nat.Prime pn ∧ pn ≠ 2) (h_sum : isSumOfTwoSmooth m pn) :
  ∃ pi : ℕ, Nat.Prime pi ∧ pi ≠ 2 ∧ pi ≤ pn ∧
  ¬ isPseudosquare (-(m : ℤ)) pi := by
rcases h_sum with ⟨a, b, hm_ab, ha_smooth, hb_smooth, ha_pos, hb_pos⟩
by_contra h_contra
simp only [ne_eq, not_exists, not_and, not_not] at h_contra
have h_contra_pn := h_contra pn h_pn.left h_pn.right (le_refl pn)
have h_mod8 := Int.ModEq.neg h_contra_pn.right.right.left
simp only [neg_neg, Int.reduceNeg] at h_mod8
-- get A from a
have hm_odd : Odd m := by
  have : (m : ℤ) % 2 = 1 := h_mod8.of_dvd (by decide)
  exact Nat.odd_iff.mpr (by exact_mod_cast this)
have h_parity : Odd a ↔ Even b := by simp_all only[ parity_simps ]
wlog ha_even: Even a
{ have := (this m b a pn h_pn); grind only [= Nat.not_odd_iff_even] }
rcases Nat.exists_eq_two_pow_mul_odd (pos_iff_ne_zero.mp ha_pos)
  with ⟨k, A, hA_odd, h_eq⟩
have hb_odd : Odd b := by grind only [= Nat.not_odd_iff_even]
let B := b
have hA_ne0 : A ≠ 0 := by grind only []
-- A and B are coprime
have hab_coprime : B.gcd A = 1 := by
  by_contra h_gcd
  obtain ⟨p, hp_prime, hp_dvd⟩ := Nat.exists_prime_and_dvd h_gcd
  have := Odd.of_dvd_nat hb_odd (Nat.gcd_dvd B A).left
  have hp_odd := Odd.of_dvd_nat this hp_dvd
  have hp_not2: p ≠ 2 := by grind only [= Nat.odd_iff]
  have hp_div_B := dvd_trans hp_dvd (Nat.gcd_dvd B A).left
  have hp_div_A := dvd_trans hp_dvd (Nat.gcd_dvd B A).right
  have hp_div_a : p ∣ a := by grind only [Nat.dvd_mul_left_of_dvd]
  have hp_div_m : p ∣ m := by
    rw [hm_ab]
    exact Dvd.dvd.add hp_div_a hp_div_B
  have hp_div_negm :  (p : ℤ) ∣ (-(m : ℤ)) := by
    rw [neg_mul_minus_1]
    apply dvd_mul_of_dvd_right
    norm_cast
  have hp_lt_pn : p ≤ pn := hb_smooth p hp_prime hp_div_B
  obtain ⟨_, _, _, h_j_is_1⟩ := h_contra p hp_prime hp_not2 hp_lt_pn
  have hp_gt0 := Nat.Prime.pos hp_prime
  have : p ≠ 0 ∧ (-(m : ℤ)).gcd p ≠ 1 := by
    constructor
    · aesop
    · have := Int.gcd_eq_right (Int.le_of_lt (by norm_cast)) hp_div_negm
      norm_cast at this
      rw [this]
      exact hp_prime.ne_one
  rw [jacobiSym.eq_zero_iff.mpr this] at h_j_is_1
  contradiction
-- two intermediate results
have h_JBA : J(-b|A) = 1 := by
  have h_prime_div_A : ∀p, Nat.Prime p → p ∣ A → p ≤ pn ∧ p ≠ 2 := by
    intros p hp p_div_A
    have hp_div_a : p ∣ a := by
      rw [h_eq]
      exact (dvd_mul_of_dvd_right p_div_A (2^k))
    constructor
    · exact ha_smooth p hp hp_div_a
    · exact Odd.ne_two_of_dvd_nat hA_odd p_div_A
  have h_Jmp_ofA : ∀p, Nat.Prime p → p ∣ A → J(-m|p) = 1 := by
    intro p hp hp_div
    have h := h_prime_div_A p hp hp_div
    obtain ⟨_, _, _, h_j_is_1⟩ := h_contra p hp h.right h.left
    exact h_j_is_1
  have h_JBp_ofA : ∀p, Nat.Prime p → p ∣ A → J(-b|p) = 1 := by
    rw [ show ( - ( b : ℤ ) ) = - ( m : ℤ ) + ( 2 ^ k * A ) by linarith ]
    intro p hp hp_div
    rw [jacobi_add_left _ _ _ (by exact_mod_cast dvd_mul_of_dvd_right hp_div (2^k))]
    exact h_Jmp_ofA p hp hp_div
  rw [jacobi_eq_prod_factorization _ _ (by assumption) (by assumption)]
  exact Finset.prod_eq_one fun p hp => by aesop
have h_JAB: J(-a|B) = 1 := by
  have h_prime_div_B : ∀p, Nat.Prime p → p ∣ B → p ≤ pn ∧ p ≠ 2 := by
    intros p hp p_div_B
    constructor
    · exact hb_smooth p hp p_div_B
    · exact Odd.ne_two_of_dvd_nat hb_odd p_div_B
  have h_Jmp_ofB : ∀p, Nat.Prime p → p ∣ B → J(-m|p) = 1 := by
    intro p hp hp_div
    have h := h_prime_div_B p hp hp_div
    exact (h_contra p hp h.right h.left).right.right.right
  have h_JmA : J(-m|B) = 1 := by
    rw [jacobi_eq_prod_factorization _ _ (ne_of_gt hb_pos) (by assumption)]
    exact Finset.prod_eq_one fun p hp => by aesop
  rw [ show ( - ( a : ℤ ) ) = - ( m : ℤ ) + ( b : ℤ ) by linarith ]
  rw [ jacobi_add_left _ _ _ (by trivial)]
  assumption
-- the main product equation
let E := (A*B-1)/2 + ((A-1)/2) * ((B-1)/2)
have h_product: J(2^k|B) * (-1)^E = 1 := by
  have intermediate : J(-1|A * B) * J(2^k|B) * J(A|B) * J(B|A) = 1 := by
    haveI : (NeZero A) := ⟨ hA_ne0 ⟩
    haveI : (NeZero B) := ⟨ ne_of_gt hb_pos ⟩
    rw [jacobiSym.mul_right]
    have : J(-a|B) * J(-b|A) = 1 := by
      simp only [h_JAB, h_JBA, mul_one]
    rw [h_eq] at this
    repeat rw [neg_mul_minus_1, jacobiSym.mul_left] at this
    rw [Int.natCast_mul, jacobiSym.mul_left] at this
    nth_rw 3 [<- this]
    norm_cast
    ring
  nth_rw 3 [Int.mul_comm] at intermediate
  have := (Odd.mul hA_odd hb_odd)
  rw [jacobiSym.at_neg_one this, ZMod.χ₄_eq_neg_one_pow (Nat.odd_iff.mp this)] at intermediate
  rw [Int.mul_assoc, jacobi_reciprocity hA_odd hb_odd hab_coprime] at intermediate
  rw [Int.mul_assoc, <- Int.pow_add] at intermediate
  rw [odd_minus_1_over_2 (A*B) this] at intermediate
  exact intermediate
-- helper functions (for grinds)
have then_E_pos : (A % 4 = 1) → (B % 4 = 1) → (-1)^E = 1 := by
  intro hA hB
  have hAB : (A * B) % 4 = 1 := by rw [Nat.mul_mod, hA, hB]
  have h_A := neg_one_pow_of_1_mod_four hA
  have h_AB := neg_one_pow_of_1_mod_four hAB
  rw [pow_add, pow_mul]
  simp only [Int.reduceNeg, h_AB, h_A, one_pow, mul_one]
have then_E_neg_A : (A % 4 = 3) → (-1)^E = -1 := by
  intro hA
  rw [pow_add, pow_mul, neg_one_pow_of_3_mod_four hA]
  have := Nat.odd_mod_four_iff.mp (Nat.odd_iff.mp hb_odd)
  cases this with
  | inl hB |inr hB =>
    grind only [neg_one_pow_of_1_mod_four, neg_one_pow_of_3_mod_four, Nat.mul_mod]
have then_E_neg_B : (B % 4 = 3) → (-1)^E = -1 := by
  intro hB
  rw [pow_add, pow_mul]
  have := Nat.odd_mod_four_iff.mp (Nat.odd_iff.mp hA_odd)
  cases this with
  | inl hA | inr hA =>
    simp only [Int.reduceNeg, Nat.mul_mod, hA, hB, one_mul, Nat.mod_succ,
      neg_one_pow_of_3_mod_four, neg_one_pow_of_1_mod_four, one_pow, mul_one]
have J_two_mod8 (r) (h1: r < 8) (h2: Odd r) (hB : B % 8 = r) :
  J(2|B) = if r = 1 ∨ r = 7 then 1 else -1 := by
    rw [jacobiSym.at_two hb_odd]
    have : r = r % 8 := (Nat.mod_eq_of_lt h1).symm
    rw [this] at hB
    have := (ZMod.natCast_eq_natCast_iff B r 8).mpr hB
    rw [this]
    match r with
    | 1 | 3 | 5 | 7 => decide
    | 0 | 2 | 4 | 6 | n+8 => contradiction
-- the end is near, doing case on k
rw [hm_ab, Nat.cast_add] at h_mod8
have h_sum7 : a + b ≡ 7 [MOD 8] := by
  exact_mod_cast (by exact h_mod8 : (a + b : ℤ) ≡ 7 [ZMOD 8])
have contra_k0 : (k = 0) → False := by
  intro hk_zero
  simp only [h_eq, hk_zero, pow_zero, one_mul] at ha_even;
  exact absurd ha_even ( by simpa using hA_odd )
have contra_k1: (k = 1) → False := by
  have contra_k11_k15 : (k = 1) → (B % 8 = 1 ∨ B % 8 = 5) → False := by
    intro hk hb
    subst_vars
    simp_all only [ Nat.ModEq ]
    grind only [= Nat.odd_iff]
  have contra_k13_k17 : (k = 1) → (B % 8 = 3 ∨ B % 8 = 7) → False := by
    intro hk hb
    have ha_mod : a % 8 = 7 - (B % 8) := by
      have := mod8_from_sum7 h_sum7
      match hb with | Or.inl h | Or.inr h => rw [h] at this ⊢; simpa
    have h_A : a % 4 = 0 := by
      match hb with | _ => omega
    grind only [= Nat.odd_iff]
  grind only [= Nat.odd_iff]
have contra_k2 :  (k = 2) → False := by
  intro hk
  rw [hk] at h_eq
  have ha_mod8 : a ≡ 4 [ZMOD 8] := by
    obtain ⟨x, hx⟩ := hA_odd
    rw [h_eq, hx]
    norm_cast
    ring_nf
    rw [add_comm, mul_comm]
    exact Nat.ModEq.modulus_mul_add
  have : B ≡ 3 [ZMOD 8] := by
    have := Int.ModEq.sub h_mod8 ha_mod8
    simp only [add_sub_cancel_left, Int.reduceNeg, Int.reduceSub] at this
    exact this
  have : B % 8 = 3 := by exact_mod_cast this
  have h_B : B % 4 = 3 := by omega
  have hE := then_E_neg_B h_B
  rw [jacobiSym.pow_left, hk, hE] at h_product
  rw [jacobi_2_pow_2_is_1 B hb_odd] at h_product
  contradiction
have contra_k3 : (n : ℕ) → (k = n + 3) → False := by
  intros n hn
  have h_a_mod8 : a ≡ 0 [MOD 8] := by
    norm_num [ Nat.ModEq, Nat.mul_mod, Nat.pow_add, Nat.pow_mul, Nat.pow_mod, hn, h_eq ]
  have h_b_mod8 : b ≡ 7 [MOD 8] := by
    unfold Nat.ModEq at h_sum7
    rw [Nat.add_mod, h_a_mod8] at h_sum7
    simp only [Nat.zero_mod, zero_add, dvd_refl, Nat.mod_mod_of_dvd, Nat.mod_succ] at h_sum7
    exact h_sum7
  have h_jacobi_k_ge_3 : J(2 ^ k | b) = 1 := by
    have := jacobiSym.pow_left 2 k b;
    rw [ this, J_two_mod8 7 (by decide) (by decide) h_b_mod8 ]
    norm_num [ hn ]
  simp_all only [ Nat.ModEq ]
  grind only []
-- the end!
match k with | 0 | 1 | 2 | n + 3 => grind only []
